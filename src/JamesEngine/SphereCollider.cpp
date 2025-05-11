#include "SphereCollider.h"

#include "Core.h"
#include "BoxCollider.h"
#include "ModelCollider.h"
#include "MathsHelper.h"

#ifdef _DEBUG
#include "Camera.h"
#include "Entity.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

namespace JamesEngine
{

#ifdef _DEBUG
	void SphereCollider::OnGUI()
	{
		if (!mDebugVisual)
			return;

		std::shared_ptr<Camera> camera = GetEntity()->GetCore()->GetCamera();

		mShader->uniform("projection", camera->GetProjectionMatrix());

		mShader->uniform("view", camera->GetViewMatrix());

		glm::mat4 mModelMatrix = glm::mat4(1.f);
		mModelMatrix = glm::translate(mModelMatrix, GetPosition() + mPositionOffset);
		glm::vec3 rotation = GetRotation() + GetRotationOffset();
		mModelMatrix = glm::rotate(mModelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		mModelMatrix = glm::rotate(mModelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		mModelMatrix = glm::rotate(mModelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		mModelMatrix = glm::scale(mModelMatrix, glm::vec3(mRadius, mRadius, mRadius));

		mShader->uniform("model", mModelMatrix);

		mShader->uniform("outlineWidth", 1.f);

		mShader->uniform("outlineColor", glm::vec3(0, 1, 0));

		glDisable(GL_DEPTH_TEST);

		mShader->drawOutline(mModel.get());

		glEnable(GL_DEPTH_TEST);
	}
#endif

	bool SphereCollider::IsColliding(std::shared_ptr<Collider> _other, glm::vec3& _collisionPoint, glm::vec3& _normal, float& _penetrationDepth)
	{
		if (_other == nullptr)
		{
			std::cout << "You should add a collider to an entity with a rigidbody" << std::endl;
			return false;
		}

		// We are sphere, other is box
		std::shared_ptr<BoxCollider> otherBox = std::dynamic_pointer_cast<BoxCollider>(_other);
		if (otherBox)
		{
			glm::vec3 boxCenter = otherBox->GetPosition() + otherBox->mPositionOffset;
			glm::vec3 boxHalfSize = otherBox->GetSize() / 2.0f;
			glm::vec3 sphereCenter = GetPosition() + mPositionOffset;
			float sphereRadius = GetRadius();

			glm::vec3 boxRotation = otherBox->GetRotation() + otherBox->GetRotationOffset();
			glm::mat4 boxRotationMatrix = glm::yawPitchRoll(glm::radians(boxRotation.y), glm::radians(boxRotation.x), glm::radians(boxRotation.z));
			glm::mat3 invBoxRotationMatrix = glm::transpose(glm::mat3(boxRotationMatrix));

			glm::vec3 localSphereCenter = invBoxRotationMatrix * (sphereCenter - boxCenter);

			glm::vec3 closestPoint = glm::clamp(localSphereCenter, -boxHalfSize, boxHalfSize);

			closestPoint = glm::vec3(boxRotationMatrix * glm::vec4(closestPoint, 1.0f)) + boxCenter;

			float distance = glm::length(closestPoint - sphereCenter);

			if (distance <= sphereRadius)
			{
				_collisionPoint = closestPoint;

				if (distance > 1e-6f)
				{
					// Sphere center is outside the box: use the vector from the closest point to the sphere center.
					_normal = glm::normalize(sphereCenter - closestPoint);
					_penetrationDepth = sphereRadius - distance;
				}
				else
				{
					// Sphere center is inside the box: compute penetration along each box axis in local space.
					glm::vec3 absLocal = glm::abs(localSphereCenter);
					glm::vec3 faceDistances = boxHalfSize - absLocal;
					float minDistance = faceDistances.x;
					int axisIndex = 0;
					if (faceDistances.y < minDistance)
					{
						minDistance = faceDistances.y;
						axisIndex = 1;
					}
					if (faceDistances.z < minDistance)
					{
						minDistance = faceDistances.z;
						axisIndex = 2;
					}
					// Determine the normal in box-local space.
					glm::vec3 localNormal(0.0f);
					localNormal[axisIndex] = (localSphereCenter[axisIndex] >= 0.0f) ? 1.0f : -1.0f;
					// Transform the local normal to world space.
					_normal = glm::normalize(glm::vec3(boxRotationMatrix * glm::vec4(localNormal, 0.0f)));
					_penetrationDepth = sphereRadius - minDistance;
				}

				return true;
			}
		}

		// We are sphere, other is sphere
		std::shared_ptr<SphereCollider> otherSphere = std::dynamic_pointer_cast<SphereCollider>(_other);
		if (otherSphere)
		{
			glm::vec3 a = GetPosition() + mPositionOffset;
			glm::vec3 b = otherSphere->GetPosition() + otherSphere->GetPositionOffset();
			float ahs = mRadius;
			float bhs = otherSphere->GetRadius();
			float distance = glm::distance(a, b);
			if (distance < ahs + bhs)
			{
				glm::vec3 direction = glm::normalize(b - a);
				_collisionPoint = a + direction * ahs;

				_normal = direction;
				_penetrationDepth = (ahs + bhs) - distance;

				return true;
			}
			return false;
		}

		// We are sphere, other is model
		std::shared_ptr<ModelCollider> otherModel = std::dynamic_pointer_cast<ModelCollider>(_other);
		if (otherModel)
		{
			// Get sphere world position and radius.
			glm::vec3 spherePos = GetPosition() + GetPositionOffset();
			float sphereRadius = GetRadius();
			float sphereRadiusSq = sphereRadius * sphereRadius;

			// Build the model's world transformation matrix.
			glm::vec3 modelPos = otherModel->GetPosition() + otherModel->GetPositionOffset();
			glm::vec3 modelScale = otherModel->GetScale();
			glm::vec3 modelRotation = otherModel->GetRotation() + otherModel->GetRotationOffset();

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, modelPos);
			modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, modelScale);

			// Test returned triangle faces of the model's BVH against the sphere.
			std::vector<Renderer::Model::Face> faces = otherModel->GetTriangles(spherePos, glm::vec3(0), glm::vec3(sphereRadius * 2));
			for (const auto& face : faces)
			{
				// Transform each vertex into world space.
				glm::vec3 a = glm::vec3(modelMatrix * glm::vec4(face.a.position, 1.0f));
				glm::vec3 b = glm::vec3(modelMatrix * glm::vec4(face.b.position, 1.0f));
				glm::vec3 c = glm::vec3(modelMatrix * glm::vec4(face.c.position, 1.0f));

				// Compute the closest point on this triangle to the sphere center.
				glm::vec3 closestPoint = Maths::ClosestPointOnTriangle(spherePos, a, b, c);

				// Check if the distance from the sphere's center to this point is within the radius.
				glm::vec3 diff = spherePos - closestPoint;
				float distanceSq = glm::dot(diff, diff);
				if (distanceSq <= sphereRadiusSq)
				{
					_collisionPoint = closestPoint;

					float distance = glm::length(diff);
					if (distance > 1e-6f)
					{
						_normal = glm::normalize(diff);
						_penetrationDepth = sphereRadius - distance;
					}
					else
					{
						// Degenerate case: sphere center is exactly on the triangle.
						// Use the triangle's face normal as the collision normal.
						glm::vec3 triNormal = glm::normalize(glm::cross(b - a, c - a));
						// Ensure the normal points from the model toward the sphere.
						if (glm::dot(spherePos - closestPoint, triNormal) < 0.0f)
							triNormal = -triNormal;
						_normal = triNormal;
						_penetrationDepth = sphereRadius;
					}

					return true;
				}
			}
		}

		return false;
	}

	glm::mat3 SphereCollider::UpdateInertiaTensor(float _mass)
	{
		return glm::mat3((2.0f / 5.0f) * _mass * mRadius * mRadius);
	}

}