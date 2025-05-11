#include "BoxCollider.h"

#include "Core.h"
#include "SphereCollider.h"
#include "ModelCollider.h"
#include "MathsHelper.h"

#include <iostream>

#ifdef _DEBUG
#include "Camera.h"
#include "Entity.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace JamesEngine
{

#ifdef _DEBUG
	void BoxCollider::OnGUI()
	{
		if (!mDebugVisual)
			return;

		std::shared_ptr<Camera> camera = GetEntity()->GetCore()->GetCamera();

		mShader->uniform("projection", camera->GetProjectionMatrix());

		mShader->uniform("view", camera->GetViewMatrix());

        glm::mat4 baseMatrix = glm::mat4(1.f);
        baseMatrix = glm::translate(baseMatrix, GetPosition() + mPositionOffset);
        baseMatrix *= glm::toMat4(GetWorldRotation());
		baseMatrix = glm::scale(baseMatrix, glm::vec3(mSize.x/2, mSize.y/2, mSize.z/2));

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(mRotationOffset.x), glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(mRotationOffset.y), glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(mRotationOffset.z), glm::vec3(0, 0, 1));
        glm::mat4 offsetMatrix{ 1.f };
        offsetMatrix = offsetMatrix * rotationMatrix;

        glm::mat4 model = baseMatrix * offsetMatrix;

		mShader->uniform("model", model);

		mShader->uniform("outlineWidth", 1.f);

		mShader->uniform("outlineColor", glm::vec3(0, 1, 0));

		glDisable(GL_DEPTH_TEST);

		mShader->drawOutline(mModel.get());

		glEnable(GL_DEPTH_TEST);
	}
#endif

	bool BoxCollider::IsColliding(std::shared_ptr<Collider> _other, glm::vec3& _collisionPoint, glm::vec3& _normal, float& _penetrationDepth)
	{
		if (_other == nullptr)
		{
			std::cout << "You should add a collider to an entity with a rigidbody" << std::endl;
			return false;
		}

		// We are box, other is box
		std::shared_ptr<BoxCollider> otherBox = std::dynamic_pointer_cast<BoxCollider>(_other);
		if (otherBox)
		{
            glm::vec3 aPos = GetPosition() + mPositionOffset;
            glm::vec3 bPos = otherBox->GetPosition() + otherBox->GetPositionOffset();
            glm::vec3 aSize = GetSize() / 2.0f;
            glm::vec3 bSize = otherBox->GetSize() / 2.0f;

            glm::vec3 aRotation = GetWorldRotationEuler() + mRotationOffset;
            glm::vec3 bRotation = otherBox->GetWorldRotationEuler() + otherBox->GetRotationOffset();

            glm::mat4 aRotationMatrix = glm::yawPitchRoll(glm::radians(aRotation.y), glm::radians(aRotation.x), glm::radians(aRotation.z));
            glm::mat4 bRotationMatrix = glm::yawPitchRoll(glm::radians(bRotation.y), glm::radians(bRotation.x), glm::radians(bRotation.z));

            glm::vec3 aAxes[3] = {
                glm::vec3(aRotationMatrix[0]),
                glm::vec3(aRotationMatrix[1]),
                glm::vec3(aRotationMatrix[2])
            };

            glm::vec3 bAxes[3] = {
                glm::vec3(bRotationMatrix[0]),
                glm::vec3(bRotationMatrix[1]),
                glm::vec3(bRotationMatrix[2])
            };

            glm::vec3 translation = bPos - aPos;
            translation = glm::vec3(glm::dot(translation, aAxes[0]), glm::dot(translation, aAxes[1]), glm::dot(translation, aAxes[2]));

            glm::mat3 rotation;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    rotation[i][j] = glm::dot(aAxes[i], bAxes[j]);
                }
            }

            glm::mat3 absRotation;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    absRotation[i][j] = std::abs(rotation[i][j]) + std::numeric_limits<float>::epsilon();
                }
            }

            for (int i = 0; i < 3; i++)
            {
                float ra = aSize[i];
                float rb = bSize[0] * absRotation[i][0] + bSize[1] * absRotation[i][1] + bSize[2] * absRotation[i][2];
                if (std::abs(translation[i]) > ra + rb) return false;
            }

            for (int i = 0; i < 3; i++)
            {
                float ra = aSize[0] * absRotation[0][i] + aSize[1] * absRotation[1][i] + aSize[2] * absRotation[2][i];
                float rb = bSize[i];
                if (std::abs(translation[0] * rotation[0][i] + translation[1] * rotation[1][i] + translation[2] * rotation[2][i]) > ra + rb)
                    return false;
            }

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    float ra = aSize[(i + 1) % 3] * absRotation[(i + 2) % 3][j] + aSize[(i + 2) % 3] * absRotation[(i + 1) % 3][j];
                    float rb = bSize[(j + 1) % 3] * absRotation[i][(j + 2) % 3] + bSize[(j + 2) % 3] * absRotation[i][(j + 1) % 3];
                    if (std::abs(translation[(i + 2) % 3] * rotation[(i + 1) % 3][j] - translation[(i + 1) % 3] * rotation[(i + 2) % 3][j]) > ra + rb)
                        return false;
                }
            }

            _collisionPoint = (aPos + bPos) / 2.0f;

            float minPenetration = std::numeric_limits<float>::max();
            glm::vec3 bestAxis(0.0f);

            // Test the 3 axes of box A (aAxes)
            for (int i = 0; i < 3; i++)
            {
                float ra = aSize[i];
                float rb = bSize[0] * absRotation[i][0] + bSize[1] * absRotation[i][1] + bSize[2] * absRotation[i][2];
                float axisProj = std::abs(translation[i]);
                float overlap = (ra + rb) - axisProj;
                if (overlap < minPenetration)
                {
                    minPenetration = overlap;
                    bestAxis = aAxes[i] * ((translation[i] < 0.0f) ? -1.0f : 1.0f);
                }
            }

            // Test the 3 axes of box B (bAxes)
            for (int i = 0; i < 3; i++)
            {
                float ra = aSize[0] * absRotation[0][i] + aSize[1] * absRotation[1][i] + aSize[2] * absRotation[2][i];
                float proj = translation[0] * rotation[0][i] + translation[1] * rotation[1][i] + translation[2] * rotation[2][i];
                float rb = bSize[i];
                float overlap = (ra + rb) - std::abs(proj);
                if (overlap < minPenetration)
                {
                    minPenetration = overlap;
                    bestAxis = bAxes[i] * ((proj < 0.0f) ? -1.0f : 1.0f);
                }
            }

            // Test the 9 cross-product axes (aAxes[i] x bAxes[j])
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    float cp = std::abs(translation[(i + 2) % 3] * rotation[(i + 1) % 3][j] - translation[(i + 1) % 3] * rotation[(i + 2) % 3][j]);
                    float ra = aSize[(i + 1) % 3] * absRotation[(i + 2) % 3][j] + aSize[(i + 2) % 3] * absRotation[(i + 1) % 3][j];
                    float rb = bSize[(j + 1) % 3] * absRotation[i][(j + 2) % 3] + bSize[(j + 2) % 3] * absRotation[i][(j + 1) % 3];
                    float overlap = (ra + rb) - cp;
                    if (overlap < minPenetration)
                    {
                        minPenetration = overlap;
                        glm::vec3 axis = glm::cross(aAxes[i], bAxes[j]);
                        if (glm::length(axis) > 0.0f)
                            axis = glm::normalize(axis);
                        // Ensure the axis points from this box to the other box.
                        float dotProd = glm::dot(bPos - aPos, axis);
                        if (dotProd < 0.0f)
                            axis = -axis;
                        bestAxis = axis;
                    }
                }
            }

            _normal = bestAxis;
            _penetrationDepth = minPenetration;

            return true;
		}

		// We are box, other is sphere
		std::shared_ptr<SphereCollider> otherSphere = std::dynamic_pointer_cast<SphereCollider>(_other);
		if (otherSphere)
		{
            glm::vec3 boxCenter = GetPosition() + mPositionOffset;
            glm::vec3 boxHalfSize = GetSize() / 2.0f;
            glm::vec3 sphereCenter = otherSphere->GetPosition() + otherSphere->mPositionOffset;
            float sphereRadius = otherSphere->GetRadius();

            glm::vec3 boxRotation = GetWorldRotationEuler() + mRotationOffset;
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

		// We are box, other is model
		std::shared_ptr<ModelCollider> otherModel = std::dynamic_pointer_cast<ModelCollider>(_other);
        if (otherModel)
        {
            // Get the box's world parameters.
            glm::vec3 boxPos = GetPosition() + GetPositionOffset();
            glm::vec3 boxRotation = GetWorldRotationEuler() + GetRotationOffset();
            glm::vec3 boxSize = GetSize();
            glm::vec3 boxHalfSize = boxSize * 0.5f;

            // Build the box's rotation matrix (from Euler angles).
            glm::mat4 boxRotMatrix = glm::mat4(1.0f);
            boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.x), glm::vec3(1, 0, 0));
            boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.y), glm::vec3(0, 1, 0));
            boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.z), glm::vec3(0, 0, 1));
            // Inverse rotation (since the rotation matrix is orthonormal, the inverse is its transpose)
            glm::mat4 invBoxRotMatrix = glm::transpose(boxRotMatrix);

            // Build the model's world transformation matrix.
            glm::vec3 modelPos = otherModel->GetPosition() + otherModel->GetPositionOffset();
            glm::vec3 modelScale = otherModel->GetScale();
            glm::vec3 modelRotation = otherModel->GetWorldRotationEuler() + otherModel->GetRotationOffset();
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, modelPos);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
            modelMatrix = glm::scale(modelMatrix, modelScale);

            std::vector<Renderer::Model::Face> faces = otherModel->GetTriangles(boxPos, boxRotation, boxSize);

            // Store all collision data
            std::vector<glm::vec3> contactPoints;
            std::vector<glm::vec3> contactNormals;

            for (const auto& face : faces)
            {
                // Transform triangle vertices into world space.
                glm::vec3 a = glm::vec3(modelMatrix * glm::vec4(face.a.position, 1.0f));
                glm::vec3 b = glm::vec3(modelMatrix * glm::vec4(face.b.position, 1.0f));
                glm::vec3 c = glm::vec3(modelMatrix * glm::vec4(face.c.position, 1.0f));

                // Transform the vertices into the box's local space.
                glm::vec3 aLocal = glm::vec3(invBoxRotMatrix * glm::vec4(a - boxPos, 1.0f));
                glm::vec3 bLocal = glm::vec3(invBoxRotMatrix * glm::vec4(b - boxPos, 1.0f));
                glm::vec3 cLocal = glm::vec3(invBoxRotMatrix * glm::vec4(c - boxPos, 1.0f));
                glm::vec3 triVerts[3] = { aLocal, bLocal, cLocal };

                // Use the SAT-based triangle-box test.
                if (Maths::TriBoxOverlap(triVerts, boxHalfSize))
                {
                    // Compute the triangle's edge cross product (used for the normal).
                    glm::vec3 crossProd = glm::cross(b - a, c - a);
                    if (glm::length(crossProd) < 1e-6f)
                        continue;

                    // 1. Compute the collision contact point: the closest point on the triangle (in world space)
                    //    to the center of the box.
                    glm::vec3 contactPoint = Maths::ClosestPointOnTriangle(boxPos, a, b, c);

                    // 2. Compute the triangle's normal in world space.
                    glm::vec3 triangleNormal = glm::normalize(crossProd);

                    // Adjust the normal so that it points from the box toward the model.
                    // We want the dot product between (contactPoint - boxPos) and the normal to be positive.
                    if (glm::dot(contactPoint - boxPos, triangleNormal) < 0.0f)
                        triangleNormal = -triangleNormal;

                    // 3. Compute the penetration depth.
                    // First, determine the box's support point along the direction opposite to the collision normal.
                    // Transform the (world-space) collision normal into the box's local space:
                    glm::vec3 localNormal = glm::vec3(invBoxRotMatrix * glm::vec4(triangleNormal, 0.0f));

                    // Store the computed collision values.
                    contactPoints.push_back(contactPoint);
                    contactNormals.push_back(triangleNormal);
                }
            }

            // If we found at least one intersection, compute the final response.
            if (!contactPoints.empty())
            {
                glm::vec3 totalPoints{ 0 };
                for (size_t i = 0; i < contactPoints.size(); ++i)
                {
                    totalPoints += contactPoints[i];
                }
                glm::vec3 averagedContactPoint{ 0 };
                averagedContactPoint.x = totalPoints.x / contactPoints.size();
                averagedContactPoint.y = totalPoints.y / contactPoints.size();
                averagedContactPoint.z = totalPoints.z / contactPoints.size();

                // Compute a weighted average normal
                glm::vec3 weightedNormal(0.0f);
                for (size_t i = 0; i < contactNormals.size(); ++i)
                {
                    weightedNormal += contactNormals[i];// *penetrationDepths[i]; // Weight by depth
                }
                float len = glm::length(weightedNormal);
                if (len > 1e-6f)
                    weightedNormal = glm::normalize(-weightedNormal);
                else
                    // fallback: just use the first contact normal
                    weightedNormal = contactNormals[0];

                glm::vec3 localNormal = glm::vec3(invBoxRotMatrix * glm::vec4(weightedNormal, 0.0f));
                glm::vec3 supportLocal;
                supportLocal.x = (localNormal.x >= 0.0f) ? -boxHalfSize.x : boxHalfSize.x;
                supportLocal.y = (localNormal.y >= 0.0f) ? -boxHalfSize.y : boxHalfSize.y;
                supportLocal.z = (localNormal.z >= 0.0f) ? -boxHalfSize.z : boxHalfSize.z;
                glm::vec3 supportWorld = boxPos + glm::vec3(boxRotMatrix * glm::vec4(supportLocal, 0.0f));
                float recalculatedPenetration = glm::dot(weightedNormal, averagedContactPoint - supportWorld);

                // Assign final values
                _collisionPoint = averagedContactPoint;
                _penetrationDepth = recalculatedPenetration;
                _normal = weightedNormal;

                //return true;
            }
        }

		return false;
	}

    glm::mat3 BoxCollider::UpdateInertiaTensor(float _mass)
    {
        glm::mat3 inertia = glm::mat3(
            (1.0f / 12.0f) * _mass * (mSize.y * mSize.y + mSize.z * mSize.z), 0, 0,
            0, (1.0f / 12.0f) * _mass * (mSize.x * mSize.x + mSize.z * mSize.z), 0,
            0, 0, (1.0f / 12.0f) * _mass * (mSize.x * mSize.x + mSize.y * mSize.y)
        );

        glm::vec3 d = mPositionOffset;
        float d2 = glm::dot(d, d);
        glm::mat3 identity(1.0f);
        glm::mat3 outer = glm::outerProduct(d, d);
        glm::mat3 correction = _mass * (d2 * identity - outer);

        inertia += correction;
        return inertia;
    }

}