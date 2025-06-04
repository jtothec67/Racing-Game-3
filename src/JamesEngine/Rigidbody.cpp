#include "Rigidbody.h"

#include "Entity.h"
#include "Core.h"
#include "Component.h"
#include "Transform.h"
#include "Collider.h"
#include "RayCollider.h"
#include "BoxCollider.h"

#include "Timer.h"

#include <vector>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/quaternion.hpp>

namespace JamesEngine
{

	void Rigidbody::OnAlive()
	{
		UpdateInertiaTensor();

		mR = glm::toMat4(GetQuaternion());
	}

	void Rigidbody::OnEarlyFixedTick()
	{
		// Step 2: Compute collisions
		// Get all colliders in the scene
		std::vector<std::shared_ptr<Collider>> colliders;
		GetEntity()->GetCore()->FindComponents(colliders);

		// Iterate through all colliders to see if we're colliding with any
		for (auto& otherCollider : colliders)
		{
			// Skip if it is ourself
			if (otherCollider->GetTransform() == GetTransform())
				continue;

			std::shared_ptr<Collider> ourCollider = GetEntity()->GetComponent<Collider>();

			glm::vec3 collisionPoint;
			glm::vec3 collisionNormal;
			float penetrationDepth;

			// Check if colliding
			if (ourCollider->IsColliding(otherCollider, collisionPoint, collisionNormal, penetrationDepth))
			{
				mCollisionPoint = collisionPoint;

				collisionNormal = glm::normalize(collisionNormal);

				// Call OnCollision for all components on both entities
				for (size_t ci = 0; ci < GetEntity()->mComponents.size(); ci++)
				{
					GetEntity()->mComponents.at(ci)->OnCollision(otherCollider->GetEntity());
				}

				for (size_t ci = 0; ci < otherCollider->GetEntity()->mComponents.size(); ci++)
				{
					otherCollider->GetEntity()->mComponents.at(ci)->OnCollision(GetEntity());
				}

				if (otherCollider->IsTrigger())
					continue;

				std::shared_ptr<Rigidbody> otherRigidbody = otherCollider->GetEntity()->GetComponent<Rigidbody>();

				// Step 3: Resolve penetration and collision

				// Two rigidbodies collided
				if (otherRigidbody)
				{
					float totalInverseMass = (1.0f / mMass) + (1.0f / otherRigidbody->GetMass());

					Move(penetrationDepth * collisionNormal * (1.0f / mMass) / totalInverseMass);

					otherRigidbody->Move(-penetrationDepth * collisionNormal * (1.0f / otherRigidbody->GetMass()) / totalInverseMass);

					ApplyImpulseResponse(otherRigidbody, collisionNormal, collisionPoint);
				}
				else // We are rigidbody, other isn't
				{
					if (!mIsStatic)
					{
						Move(penetrationDepth * collisionNormal);

						ApplyImpulseResponseStatic(collisionNormal, collisionPoint);

						if (std::dynamic_pointer_cast<RayCollider>(ourCollider))
						{
							glm::vec3 velocityAtPoint = mVelocity;

							// Project velocity onto normal
							float normalVelocity = glm::dot(velocityAtPoint, collisionNormal);

							if (normalVelocity < 0.0f) // Only if moving into the surface
							{
								float restitution = GetRestitution();

								float impulseMagnitude = -(1.0f + restitution) * normalVelocity;
								impulseMagnitude /= 1.f / GetMass();

								glm::vec3 impulse = impulseMagnitude * collisionNormal;

								ApplyImpulse(impulse);
							}
						}
					}
				}
			}
		}
	}

	void Rigidbody::OnFixedTick()
	{
		if (!mIsStatic)
		{
			// Step 1: Compute each of the forces acting on the object (only gravity by default)
			glm::vec3 force = mMass * mAcceleration;
			AddForce(force);

			// Step 5: Integration
			//Euler();
			SemiImplicitEuler();
			//Verlet();
		}

		// Step 7: Clear forces
		ClearForces();
	}

	void Rigidbody::ApplyImpulseResponse(std::shared_ptr<Rigidbody> _other, glm::vec3 _normal, glm::vec3 _collisionPoint)
	{
		// --- Precompute values for both bodies ---
		float invMassA = (GetMass() > 0.0f) ? (1.0f / GetMass()) : 0.0f;
		float invMassB = (_other->GetMass() > 0.0f) ? (1.0f / _other->GetMass()) : 0.0f;

		// Compute lever arms from centers of mass to collision point.
		glm::vec3 ra = _collisionPoint - GetPosition();
		glm::vec3 rb = _collisionPoint - _other->GetPosition();

		// Calculate velocities at the contact point (including rotation).
		glm::vec3 vA = GetVelocity() + glm::cross(GetAngularVelocity(), ra);
		glm::vec3 vB = _other->GetVelocity() + glm::cross(_other->GetAngularVelocity(), rb);

		// Relative velocity at contact point.
		glm::vec3 relativeVel = vA - vB;
		float relVelAlongNormal = glm::dot(relativeVel, _normal);

		// Do not resolve if moving apart.
		if (relVelAlongNormal > 0.0f)
			return;

		// --- Effective mass (including rotation) for normal impulse ---
		// Compute the effect of angular inertia.
		glm::vec3 raCrossN = glm::cross(ra, _normal);
		glm::vec3 rbCrossN = glm::cross(rb, _normal);
		float angularEffectA = glm::dot(_normal, glm::cross(mInertiaTensorInverse * raCrossN, ra));
		float angularEffectB = glm::dot(_normal, glm::cross(_other->mInertiaTensorInverse * rbCrossN, rb));

		// Bounce factor (elasticity).
		float restitution = std::min(GetRestitution(), _other->GetRestitution());

		// Compute impulse scalar (j) for the collision along the normal.
		float j = -(1.0f + restitution) * relVelAlongNormal;
		j /= (invMassA + invMassB + angularEffectA + angularEffectB);

		// Normal impulse.
		glm::vec3 impulse = j * _normal;

		// Apply linear impulse.
		SetVelocity(GetVelocity() + impulse * invMassA);
		_other->SetVelocity(_other->GetVelocity() - impulse * invMassB);

		// Apply angular impulse.
		if (!GetLockRotation())
			SetAngularMomentum(GetAngularMomentum() + glm::cross(ra, impulse));

		if (!_other->GetLockRotation())
			_other->SetAngularMomentum(_other->GetAngularMomentum() - glm::cross(rb, impulse));

		// --- Friction Impulse ---
		// Recompute contact velocities with updated velocities.
		vA = GetVelocity() + glm::cross(GetAngularVelocity(), ra);
		vB = _other->GetVelocity() + glm::cross(_other->GetAngularVelocity(), rb);
		relativeVel = vA - vB;

		// Compute the contact tangent direction.
		glm::vec3 tangent = relativeVel - glm::dot(relativeVel, _normal) * _normal;
		float tangentialSpeed = glm::length(tangent);
		if (tangentialSpeed > 1e-6f)
		{
			tangent = tangent / tangentialSpeed;
		}
		else
		{
			tangent = glm::vec3(0.0f);
		}

		// Effective mass for friction (similar to the normal calculation).
		glm::vec3 raCrossT = glm::cross(ra, tangent);
		glm::vec3 rbCrossT = glm::cross(rb, tangent);
		float angularEffectA_T = glm::dot(tangent, glm::cross(mInertiaTensorInverse * raCrossT, ra));
		float angularEffectB_T = glm::dot(tangent, glm::cross(_other->mInertiaTensorInverse * rbCrossT, rb));

		float jt = -glm::dot(relativeVel, tangent);
		jt /= (invMassA + invMassB + angularEffectA_T + angularEffectB_T);

		// Friction coefficient.
		float frictionCoefficient = (GetFriction() + _other->GetFriction()) / 2.0f;

		// Clamp friction to Coulomb’s friction law.
		glm::vec3 frictionImpulse;
		if (glm::abs(jt) <= j * frictionCoefficient)
			frictionImpulse = jt * tangent;
		else
			frictionImpulse = -j * frictionCoefficient * tangent;

		// Apply friction impulses.
		SetVelocity(GetVelocity() + frictionImpulse * invMassA);
		_other->SetVelocity(_other->GetVelocity() - frictionImpulse * invMassB);

		if (!GetLockRotation())
			SetAngularMomentum(GetAngularMomentum() + glm::cross(ra, frictionImpulse));

		if (!_other->GetLockRotation())
			_other->SetAngularMomentum(_other->GetAngularMomentum() - glm::cross(rb, frictionImpulse));
	}

	void Rigidbody::ApplyImpulseResponseStatic(glm::vec3 _normal, glm::vec3 _collisionPoint)
	{
		// Compute the vector from the center of mass to the collision point.
		glm::vec3 r = _collisionPoint - GetPosition();
    
		// Calculate the velocity at the collision point (linear plus rotational).
		glm::vec3 v = GetVelocity() + glm::cross(GetAngularVelocity(), r);
    
		// Determine the component of the velocity along the collision normal.
		float velAlongNormal = glm::dot(v, _normal);
    
		// If the dynamic object is moving away from the static object, no impulse is needed.
		if (velAlongNormal > 0)
			return;
    
		// Use the object's restitution value.
		float restitution = GetRestitution();
    
		// Since the static object doesn't move, its inverse mass is 0.
		float invMassSum = 1.0f / GetMass();
    
		// Compute the rotational inertia term.
		glm::vec3 r_cross_n = glm::cross(r, _normal);
		glm::vec3 angularComponent = mInertiaTensorInverse * r_cross_n;
		float angularTerm = glm::dot(_normal, glm::cross(angularComponent, r));
    
		// Calculate the impulse scalar.
		float j = -(1.0f + restitution) * velAlongNormal / (invMassSum + angularTerm);
    
		// Compute the impulse vector along the normal.
		glm::vec3 impulse = j * _normal;
    
		// Apply the impulse: update linear and angular velocities.
		SetVelocity(GetVelocity() + impulse * (1.0f / GetMass()));

		if (!GetLockRotation())
		{
			//SetAngularMomentum(GetAngularMomentum() + glm::cross(impulse, r));
			SetAngularMomentum(GetAngularMomentum() + glm::cross(r, impulse));
			SetAngularVelocity(GetAngularVelocity() + mInertiaTensorInverse * GetAngularMomentum());
		}
    
		// --- Coulomb Friction ---
		// Recalculate the velocity at the collision point after applying the normal impulse.
		v = GetVelocity() + glm::cross(GetAngularVelocity(), r);
    
		// The static object has zero velocity, so the relative velocity is simply v.
		glm::vec3 relativeVelocity = v;
    
		// Determine the tangent direction by removing the normal component.
		glm::vec3 tangent = relativeVelocity - glm::dot(relativeVelocity, _normal) * _normal;
		if (glm::length(tangent) > 0.0001f)
			tangent = glm::normalize(tangent);
		else
			return; // No significant tangential motion; skip friction.
    
		// Calculate the friction impulse scalar.
		float jt = -glm::dot(relativeVelocity, tangent);
		jt /= (1.0f / GetMass()) + glm::dot(tangent, glm::cross(mInertiaTensorInverse * glm::cross(r, tangent), r));
    
		// Use the object's friction coefficient
		float mu = GetFriction();
    
		// Clamp the friction impulse to the Coulomb friction cone.
		if (fabs(jt) > mu * fabs(j))
			jt = mu * fabs(j) * (jt < 0 ? -1.0f : 1.0f);
    
		// Compute the friction impulse vector.
		glm::vec3 frictionImpulse = jt * tangent;
    
		// Apply the friction impulse.
		SetVelocity(GetVelocity() + frictionImpulse * (1.0f / GetMass()));

		if (!GetLockRotation())
		{
			//SetAngularMomentum(GetAngularMomentum() + glm::cross(frictionImpulse, r));
			SetAngularMomentum(GetAngularMomentum() + glm::cross(r, frictionImpulse));
			SetAngularVelocity(GetAngularVelocity() + mInertiaTensorInverse * GetAngularMomentum());
		}
	}

	glm::vec3 Rigidbody::FrictionForce(glm::vec3 _relativeVelocity, glm::vec3 _contactNormal, glm::vec3 _forceNormal, float mu)
	{
		glm::vec3 tangential = _relativeVelocity - glm::dot(_relativeVelocity, _contactNormal) * _contactNormal;
		float speed = glm::length(tangential);

		if (speed > 1e-6f)
		{
			// Friction acts opposite to the sliding direction.
			glm::vec3 frictionDirection = -glm::normalize(tangential);
			// Coulomb friction: magnitude = mu * |normal force|
			return frictionDirection * mu * glm::length(_forceNormal);
		}

		return glm::vec3(0.0f);
	}

	glm::vec3 Rigidbody::ComputeTorque(glm::vec3 torque_arm, glm::vec3 contact_force)
	{
		glm::vec3 torque = glm::cross(contact_force, torque_arm);

		return torque;
	}

	void Rigidbody::Euler()
	{
		float dt = GetCore()->FixedDeltaTime();

		// --- Linear Dynamics ---
		// Compute inverse mass (if mass is zero, the body is static).
		float invMass = (mMass > 0.0f) ? (1.0f / mMass) : 0.0f;

		// Update linear velocity from forces.
		mVelocity += (mForce * invMass) * dt;
		// Update position.
		Move(mVelocity * dt);

		// --- Angular Dynamics ---
		mAngularMomentum += mTorque * dt;
		// Recompute the inverse inertia tensor (if it depends on orientation or other factors)
		ComputeInverseInertiaTensor();
		mAngularVelocity = mInertiaTensorInverse * mAngularMomentum;

		// Integrate orientation using quaternions:
		// Get the current quaternion (representing orientation)
		glm::quat q = GetQuaternion();
		// Form the quaternion representing angular velocity (with a zero real part)
		glm::quat omegaQuat(0.0f, mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z);
		glm::quat dq = 0.5f * omegaQuat * q;
		// Update the quaternion using the new derivative
		q += dq * dt;
		// Normalize to prevent drift and rounding errors
		q = glm::normalize(q);
		// Set the updated orientation
		SetQuaternion(q);

		// Update the rotation matrix for convenience (if needed)
		mR = glm::mat3_cast(q);
	}

	void Rigidbody::SemiImplicitEuler()
	{
		float dt = GetCore()->FixedDeltaTime();

		// ----- Linear Integration -----
		glm::vec3 acceleration = mForce / mMass;
		mVelocity += acceleration * dt;

		mVelocity *= mLinearDamping;

		SetPosition(GetPosition() + mVelocity * dt);

		// ----- Angular Integration -----
		mAngularMomentum += mTorque * dt;
		// Recompute the inverse inertia tensor
		ComputeInverseInertiaTensor();
		mAngularVelocity = mInertiaTensorInverse * mAngularMomentum;

		mAngularMomentum *= mAngularDamping;

		// Already broken if spinning this fast
		const glm::vec3 maxAngularVelocity(90.0f);
		const glm::vec3 maxAngularMomentum = glm::inverse(mInertiaTensorInverse) * maxAngularVelocity;
		mAngularMomentum = glm::clamp(mAngularMomentum, -maxAngularMomentum, maxAngularMomentum);

		mAngularVelocity = glm::clamp(mAngularVelocity, glm::vec3(-90.0f), glm::vec3(90.0f));

		// Integrate orientation using quaternions:
		// Get the current quaternion (representing orientation)
		glm::quat q = GetQuaternion();
		// Form the quaternion representing angular velocity (with a zero real part)
		glm::quat omegaQuat(0.0f, mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z);
		glm::quat dq = 0.5f * omegaQuat * q;
		// Update the quaternion using the new derivative
		q += dq * dt;
		// Normalize to prevent drift and rounding errors
		q = glm::normalize(q);
		// Set the updated orientation
		SetQuaternion(q);

		// Update the rotation matrix
		mR = glm::mat3_cast(q);
	}


	void Rigidbody::Verlet()
	{
		float dt = GetCore()->FixedDeltaTime();

		glm::vec3 acceleration = mForce / mMass;
		mPreviousPosition = GetPosition() - mVelocity * dt + 0.5f * acceleration * dt * dt;
		SetPosition(2.0f * GetPosition() - mPreviousPosition + acceleration * dt * dt);
		mVelocity = (GetPosition() - mPreviousPosition) / (2.0f * dt);
		mVelocity += acceleration * dt;

		// Angular motion update
		mAngularMomentum += mTorque * dt;
		// Recompute the inverse inertia tensor (if it depends on orientation or other factors)
		ComputeInverseInertiaTensor();
		mAngularVelocity = mInertiaTensorInverse * mAngularMomentum;

		// Integrate orientation using quaternions:
		// Get the current quaternion (representing orientation)
		glm::quat q = GetQuaternion();
		// Form the quaternion representing angular velocity (with a zero real part)
		glm::quat omegaQuat(0.0f, mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z);
		glm::quat dq = 0.5f * omegaQuat * q;
		// Update the quaternion using the new derivative
		q += dq * dt;
		// Normalize to prevent drift and rounding errors
		q = glm::normalize(q);
		// Set the updated orientation
		SetQuaternion(q);

		// Update the rotation matrix for convenience (if needed)
		mR = glm::mat3_cast(q);
	}

	void Rigidbody::UpdateInertiaTensor()
	{
		glm::mat3 bodyInertia = glm::mat3(0.0f);

		if (mUsingCustomInertia)
			bodyInertia = GetEntity()->GetComponent<Collider>()->UpdateInertiaTensor(mCustomInertiaMass);
		else
			bodyInertia = GetEntity()->GetComponent<Collider>()->UpdateInertiaTensor(mMass);

		mBodyInertiaTensorInverse = glm::inverse(bodyInertia);

		ComputeInverseInertiaTensor();
	}

	void Rigidbody::ComputeInverseInertiaTensor()
	{
		//mInertiaTensorInverse = mR * mBodyInertiaTensorInverse * glm::transpose(mR);
		glm::mat3 R = glm::mat3_cast(GetQuaternion());
		mInertiaTensorInverse = R * mBodyInertiaTensorInverse * glm::transpose(R);
	}

}