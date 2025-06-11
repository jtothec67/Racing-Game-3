#include "RayCollider.h"

#include "Core.h"
#include "ModelCollider.h"
#include "MathsHelper.h"
#include "Entity.h"
#include "Suspension.h"
#include "Tire.h"

#include <iostream>

#ifdef _DEBUG
#include "Camera.h"
#include "Entity.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

namespace JamesEngine
{

#ifdef _DEBUG
    void RayCollider::OnGUI()
    {
        if (!mDebugVisual)
            return;

        std::shared_ptr<Camera> camera = GetEntity()->GetCore()->GetCamera();

        mShader->uniform("projection", camera->GetProjectionMatrix());

        mShader->uniform("view", camera->GetViewMatrix());

        glm::vec3 rayOrigin = GetPosition() + mPositionOffset;

        glm::vec3 localRayDir = GetDirection();
        glm::vec3 entityRotation = GetRotation() + GetRotationOffset();
        glm::mat4 rayRotationMatrix = glm::mat4(1.0f);
        rayRotationMatrix = glm::rotate(rayRotationMatrix, glm::radians(entityRotation.x), glm::vec3(1, 0, 0));
        rayRotationMatrix = glm::rotate(rayRotationMatrix, glm::radians(entityRotation.y), glm::vec3(0, 1, 0));
        rayRotationMatrix = glm::rotate(rayRotationMatrix, glm::radians(entityRotation.z), glm::vec3(0, 0, 1));
        glm::vec3 rayDirection = glm::normalize(glm::vec3(rayRotationMatrix * glm::vec4(localRayDir, 0.0f)));

        glm::mat4 mModelMatrix = glm::translate(glm::mat4(1.0f), rayOrigin);

        glm::vec3 up = glm::vec3(0, 1, 0);
        float dotVal = glm::dot(up, rayDirection);
        glm::vec3 rotationAxis = glm::cross(up, rayDirection);
        float angle = acos(glm::clamp(dotVal, -1.0f, 1.0f));
        if (glm::length(rotationAxis) < 1e-6) {
            if (dotVal < 0.0f) {
                rotationAxis = glm::vec3(1, 0, 0);
                angle = glm::pi<float>();
            }
        }
        else {
            rotationAxis = glm::normalize(rotationAxis);
        }
        mModelMatrix = mModelMatrix * glm::rotate(glm::mat4(1.0f), angle, rotationAxis);

        glm::mat4 T_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1.425f, 0));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, mLength / 2.85f, 0.05f));
        glm::mat4 localTransform = S * T_offset;

        mModelMatrix = mModelMatrix * localTransform;

        mShader->uniform("model", mModelMatrix);

        mShader->uniform("outlineWidth", 1.f);

        mShader->uniform("outlineColor", glm::vec3(0, 1, 0));

        glDisable(GL_DEPTH_TEST);

        mShader->drawOutline(mModel.get());

        glEnable(GL_DEPTH_TEST);
    }
#endif

    bool RayCollider::IsColliding(std::shared_ptr<Collider> _other, glm::vec3& _collisionPoint, glm::vec3& _normal, float& _penetrationDepth)
    {
        if (_other == nullptr)
        {
            std::cout << "You should add a collider to an entity with a rigidbody" << std::endl;
            return false;
        }

        // We are ray, other is model
        std::shared_ptr<ModelCollider> otherModel = std::dynamic_pointer_cast<ModelCollider>(_other);
        if (otherModel)
        {
            // Compute ray's world-space origin.
            glm::vec3 rayOrigin = GetPosition() + GetPositionOffset();

            // Transform the ray's local direction into world space.
            glm::vec3 localRayDir = GetDirection();
            glm::vec3 entityRotation = GetRotation() + GetRotationOffset();
            glm::mat4 rayRotationMatrix = glm::mat4(1.0f);
            rayRotationMatrix = glm::rotate(rayRotationMatrix, glm::radians(entityRotation.x), glm::vec3(1, 0, 0));
            rayRotationMatrix = glm::rotate(rayRotationMatrix, glm::radians(entityRotation.y), glm::vec3(0, 1, 0));
            rayRotationMatrix = glm::rotate(rayRotationMatrix, glm::radians(entityRotation.z), glm::vec3(0, 0, 1));
            glm::vec3 rayDirection = glm::normalize(glm::vec3(rayRotationMatrix * glm::vec4(localRayDir, 0.0f)));

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

            // Compute the ray's endpoint
            glm::vec3 rayEnd = rayOrigin + rayDirection * mLength;

            // Compute min and max of the ray's endpoints.
            glm::vec3 bbMin = glm::min(rayOrigin, rayEnd);
            glm::vec3 bbMax = glm::max(rayOrigin, rayEnd);

            // Calculate the center and size (extents) of the AABB.
            glm::vec3 bbCenter = (bbMin + bbMax) * 0.5f;
            glm::vec3 bbSize = bbMax - bbMin;

            // Retrieve the triangles from the model that lie within the ray's AABB.
            std::vector<Renderer::Model::Face> faces = otherModel->GetTriangles(bbCenter, glm::vec3(0), bbSize);

            bool hit = false;
            float closestT = mLength;
            glm::vec3 hitPoint, hitNormal;

            // Test each triangle for intersection with the ray.
            for (const auto& face : faces)
            {
                // Transform triangle vertices to world space.
                glm::vec3 a = glm::vec3(modelMatrix * glm::vec4(face.a.position, 1.0f));
                glm::vec3 b = glm::vec3(modelMatrix * glm::vec4(face.b.position, 1.0f));
                glm::vec3 c = glm::vec3(modelMatrix * glm::vec4(face.c.position, 1.0f));

                // Test for ray-triangle intersection.
                float t, u, v;
                if (Maths::RayTriangleIntersect(rayOrigin, rayDirection, a, b, c, t, u, v))
                {
                    // Ensure the hit is in front of the ray origin and is the closest so far.
                    if (t >= 0.0f && t < closestT && t <= mLength)
                    {
                        closestT = t;
                        hitPoint = rayOrigin + rayDirection * t;
                        // Compute the triangle's normal.
                        hitNormal = glm::normalize(glm::cross(b - a, c - a));
                        // Ensure the normal points against the ray direction.
                        if (glm::dot(rayDirection, hitNormal) > 0.0f)
                            hitNormal = -hitNormal;
                        hit = true;
                    }
                }
            }

            if (hit)
            {
                float surfaceAlignment = glm::dot(hitNormal, -rayDirection);

                float groundPenetration = mLength - closestT;

                // Should bounce off wall, not stand on top
                if ((surfaceAlignment <= mSteepnessThreshold) && (groundPenetration >= mLength * mMinPenetrationPercentage))
                {
                    glm::vec3 objectToHit = hitPoint - rayOrigin;
                    _penetrationDepth = glm::dot(objectToHit, hitNormal);
                    _normal = hitNormal;
                }
                else
                {
                    _penetrationDepth = groundPenetration;
                    _normal = hitNormal;
                }

                _collisionPoint = hitPoint;

				std::shared_ptr<Suspension> sus = GetEntity()->GetComponent<Suspension>();
                if (sus)
                {
                    sus->SetCollision(true);
                    sus->SetHitDistance(glm::dot(hitPoint - rayOrigin, rayDirection));
					sus->SetSurfaceNormal(hitNormal);
					sus->SetContactPoint(hitPoint);
                }

				std::shared_ptr<Tire> tire = GetEntity()->GetComponent<Tire>();
				if (tire)
				{
					tire->SetTireContactPoint(hitPoint);
				}

                return true;
            }
        }

        return false;
    }

}