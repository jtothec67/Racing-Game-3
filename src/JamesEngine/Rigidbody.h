#pragma once

#include "Component.h"

namespace JamesEngine
{

	class Rigidbody : public Component
	{
	public:
		void OnAlive();
		void OnEarlyFixedTick();
		void OnFixedTick();

		void AddForce(glm::vec3 _force) { mForce += _force; }
		void AddTorque(glm::vec3 _torque) { mTorque += _torque; }
		void ClearForces() { mForce = glm::vec3(0); mTorque = glm::vec3(0); }

		void ApplyImpulse(glm::vec3 _impulse) { mVelocity += _impulse / mMass; }
		void ApplyTorqueImpulse(glm::vec3 _impulse) { mAngularMomentum += _impulse; }

		void ApplyForce(glm::vec3 _force, glm::vec3 _point) { mForce += _force; mTorque += glm::cross(_point - GetPosition(), _force); }

		void SetMass(float _mass) { mMass = _mass; UpdateInertiaTensor(); }
		float GetMass() { return mMass; }

		void SetFriction(float _friction) { mFriction = glm::clamp(_friction, 0.f, 1.f); }
		float GetFriction() { return mFriction; }

		void SetRestitution(float _restitution) { mRestitution = glm::clamp(_restitution, 0.f, 1.f); }
		float GetRestitution() { return mRestitution; }

		void SetForce(glm::vec3 _force) { mForce = _force; }
		glm::vec3 GetForce() { return mForce; }

		void SetTorque(glm::vec3 _torque) { mTorque = _torque; }
		glm::vec3 GetTorque() { return mTorque; }

		void SetVelocity(glm::vec3 _velocity) { mVelocity = _velocity; }
		glm::vec3 GetVelocity() { return mVelocity; }

		void SetAcceleration(glm::vec3 _acceleration) { mAcceleration = _acceleration; }
		glm::vec3 GetAcceleration() { return mAcceleration; }

		void SetAngularMomentum(glm::vec3 _angularMomentum) { mAngularMomentum = _angularMomentum; }
		glm::vec3 GetAngularMomentum() { return mAngularMomentum; }

		void SetAngularVelocity(glm::vec3 _angularVelocity) { mAngularVelocity = _angularVelocity; }
		glm::vec3 GetAngularVelocity() { return mAngularVelocity; }

		glm::vec3 GetVelocityAtPoint(glm::vec3 _point) { return mVelocity + glm::cross(mAngularVelocity, _point - GetPosition()); }

		glm::vec3 mCollisionPoint = glm::vec3(0);

		void LockRotation(bool _lock) { mLockRotation = _lock; }
		bool GetLockRotation() { return mLockRotation; }

		void ApplyImpulseResponseStatic(glm::vec3 _normal, glm::vec3 _collisionPoint);

		void IsStatic(bool _isStatic) { mIsStatic = _isStatic; }
		bool IsStatic() { return mIsStatic; }

		void SetCustomInertiaMass(float _mass) { mCustomInertiaMass = _mass; mUsingCustomInertia = true; }
		float GetCustomInertiaMass() { return mCustomInertiaMass; }
	private:

		void ApplyImpulseResponse(std::shared_ptr<Rigidbody> _other, glm::vec3 _normal, glm::vec3 _collisionPoint);
		
		glm::vec3 FrictionForce(glm::vec3 _relativeVelocity, glm::vec3 _contactNormal, glm::vec3 _forceNormal, float mu);
		glm::vec3 ComputeTorque(glm::vec3 torque_arm, glm::vec3 contact_force);

		void UpdateInertiaTensor();
		void ComputeInverseInertiaTensor();

		void Euler();
		void SemiImplicitEuler();
		void Verlet();

		glm::vec3 mForce = glm::vec3(0);
		glm::vec3 mVelocity = glm::vec3(0);
		glm::vec3 mAcceleration = glm::vec3(0, -9.81, 0);
		glm::vec3 mPreviousPosition = glm::vec3(0);

		float mMass = 1.f;
		float mFriction = 0.9f;
		float mRestitution = 0.1f;

		glm::vec3 mTorque = glm::vec3{ 0 };
		glm::vec3 mAngularVelocity = glm::vec3{ 0 };
		glm::vec3 mAngularMomentum = glm::vec3{ 0 };
		glm::mat3 mInertiaTensorInverse = glm::mat3{ 0 };
		glm::mat3 mBodyInertiaTensorInverse = glm::mat3{ 0 };
		glm::mat3 mR = glm::mat3(	1.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 1.0f);

		float mLinearDamping = 1.f;
		float mAngularDamping = 1.f;

		bool mLockRotation = false;

		bool mIsStatic = false;

		bool mUsingCustomInertia = false;
		float mCustomInertiaMass = 1.f;
	};

}