#include "ModelCollider.h"

#include "Core.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

#include "MathsHelper.h"

#include <iostream>
#include <algorithm>
#include <iomanip>

#ifdef _DEBUG
#include "Camera.h"
#include "Entity.h"
#include "Transform.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

namespace JamesEngine
{

#ifdef _DEBUG
    void ModelCollider::OnGUI()
    {
        if (!mDebugVisual)
            return;

        if (mModel == nullptr)
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
        mModelMatrix = glm::scale(mModelMatrix, glm::vec3(GetScale().x, GetScale().y, GetScale().z));

        mShader->uniform("model", mModelMatrix);

        mShader->uniform("outlineWidth", 1.f);

        mShader->uniform("outlineColor", glm::vec3(0, 1, 0));

        glDisable(GL_DEPTH_TEST);

        mShader->drawOutline(mModel->mModel.get());

        glEnable(GL_DEPTH_TEST);
    }
#endif

    void ModelCollider::OnAlive()
    {
        if (mModel == nullptr)
        {
            std::cout << "You need to add a model to the model collider" << std::endl;
            return;
        }

		// Build the BVH from the model's triangles.
        mBVHRoot = BuildBVH(mModel->mModel->GetFaces(), mBVHLeafThreshold);
        const std::vector<Renderer::Model::Face>& faces = mModel->mModel->GetFaces();
    }

    bool ModelCollider::IsColliding(std::shared_ptr<Collider> _other, glm::vec3& _collisionPoint, glm::vec3& _normal, float& _penetrationDepth)
    {
        if (_other == nullptr)
        {
            std::cout << "You should add a collider to an entity with a rigidbody" << std::endl;
            return false;
        }

        if (mModel == nullptr)
        {
            return false;
        }

        // We are model, other is sphere
        std::shared_ptr<SphereCollider> otherSphere = std::dynamic_pointer_cast<SphereCollider>(_other);
        if (otherSphere)
        {
            // Get sphere world position and radius.
            glm::vec3 spherePos = otherSphere->GetPosition() + otherSphere->GetPositionOffset();
            float sphereRadius = otherSphere->GetRadius();
            float sphereRadiusSq = sphereRadius * sphereRadius;

            // Build the model's world transformation matrix.
            glm::vec3 modelPos = GetPosition() + GetPositionOffset();
            glm::vec3 modelScale = GetScale();
            glm::vec3 modelRotation = GetRotation() + GetRotationOffset();

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, modelPos);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
            modelMatrix = glm::scale(modelMatrix, modelScale);

            // Test returned triangle faces of the model's BVH against the sphere.
            std::vector<Renderer::Model::Face> faces = GetTriangles(spherePos, glm::vec3(0), glm::vec3(sphereRadius * 2));
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
                }
            }
        }

        // We are model, other is box
        std::shared_ptr<BoxCollider> otherBox = std::dynamic_pointer_cast<BoxCollider>(_other);
        if (otherBox)
        {
            // Get the box's world parameters.
            glm::vec3 boxPos = otherBox->GetPosition() + otherBox->GetPositionOffset();
            glm::vec3 boxRotation = otherBox->GetRotation() + otherBox->GetRotationOffset();
            glm::vec3 boxSize = otherBox->GetSize();
            glm::vec3 boxHalfSize = boxSize * 0.5f;

            // Build the box's rotation matrix (from Euler angles).
            glm::mat4 boxRotMatrix = glm::mat4(1.0f);
            boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.x), glm::vec3(1, 0, 0));
            boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.y), glm::vec3(0, 1, 0));
            boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.z), glm::vec3(0, 0, 1));
            // Inverse rotation (since the rotation matrix is orthonormal, the inverse is its transpose)
            glm::mat4 invBoxRotMatrix = glm::transpose(boxRotMatrix);

            // Build the model's world transformation matrix.
            glm::vec3 modelPos = GetPosition() + GetPositionOffset();
            glm::vec3 modelScale = GetScale();
            glm::vec3 modelRotation = GetRotation() + GetRotationOffset();
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, modelPos);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
            modelMatrix = glm::scale(modelMatrix, modelScale);

            // Test returned triangle faces of the model's BVH against the box.
            std::vector<Renderer::Model::Face> faces = GetTriangles(boxPos, boxRotation, boxSize);
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
                    // Compute an approximate collision point.
                    // Here we take the closest point on the triangle (in world space)
                    // to the box center.
                    glm::vec3 closestPoint = Maths::ClosestPointOnTriangle(boxPos, a, b, c);
                    _collisionPoint = closestPoint;

                    // Compute the cross product
                    glm::vec3 crossProd = glm::cross(b - a, c - a);
                    // Check if the cross product is near zero (degenerate triangle)
                    if (glm::length(crossProd) < 1e-6f)
                        continue;

                    // Compute the triangle's normal in world space.
                    glm::vec3 triNormal = glm::normalize(crossProd);
                    // Ensure the normal points from the model (triangle) toward the box.
                    if (glm::dot(boxPos - closestPoint, triNormal) < 0.0f)
                        triNormal = -triNormal;
                    _normal = triNormal;

                    // To compute penetration depth, determine the box's support point
                    // in the direction opposite to the collision normal.
                    glm::vec3 localNormal = glm::vec3(invBoxRotMatrix * glm::vec4(_normal, 0.0f));
                    glm::vec3 supportLocal{};
                    supportLocal.x = (localNormal.x >= 0.0f) ? -boxHalfSize.x : boxHalfSize.x;
                    supportLocal.y = (localNormal.y >= 0.0f) ? -boxHalfSize.y : boxHalfSize.y;
                    supportLocal.z = (localNormal.z >= 0.0f) ? -boxHalfSize.z : boxHalfSize.z;
                    glm::vec3 supportWorld = boxPos + glm::vec3(boxRotMatrix * glm::vec4(supportLocal, 0.0f));

                    // Penetration depth is approximated as the projection of the vector from the support point
                    // to the collision point along the collision normal.
                    _penetrationDepth = glm::dot(_normal, _collisionPoint - supportWorld);

                    return true;
                }
            }
        }

        // We are model, other is model
        std::shared_ptr<ModelCollider> otherModel = std::dynamic_pointer_cast<ModelCollider>(_other);
        if (otherModel)
        {
            // Build world transform for "this" model.
            glm::vec3 modelPos = GetPosition() + GetPositionOffset();
            glm::vec3 modelScale = GetScale();
            glm::vec3 modelRotation = GetRotation();// + GetRotationOffset();
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, modelPos);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
            modelMatrix = glm::scale(modelMatrix, modelScale);

            // Build world transform for the other model.
            glm::vec3 otherModelPos = otherModel->GetPosition() + otherModel->GetPositionOffset();
            glm::vec3 otherModelScale = otherModel->GetScale();
            glm::vec3 otherModelRotation = otherModel->GetRotation();// + otherModel->GetRotationOffset();
            glm::mat4 otherModelMatrix = glm::mat4(1.0f);
            otherModelMatrix = glm::translate(otherModelMatrix, otherModelPos);
            otherModelMatrix = glm::rotate(otherModelMatrix, glm::radians(otherModelRotation.x), glm::vec3(1, 0, 0));
            otherModelMatrix = glm::rotate(otherModelMatrix, glm::radians(otherModelRotation.y), glm::vec3(0, 1, 0));
            otherModelMatrix = glm::rotate(otherModelMatrix, glm::radians(otherModelRotation.z), glm::vec3(0, 0, 1));
            otherModelMatrix = glm::scale(otherModelMatrix, otherModelScale);

            // Get faces for each model.
            glm::vec3 thisBoxPos = GetPosition();// + GetPositionOffset();
            glm::vec3 thisBoxRotation = GetRotation();// + GetRotationOffset();
            glm::vec3 thisBoxSize = glm::vec3((mModel->mModel->get_width() * GetScale().x), (mModel->mModel->get_height() * GetScale().y), (mModel->mModel->get_length() * GetScale().z));

            glm::vec3 otherBoxPos = otherModel->GetPosition();// +otherModel->GetPositionOffset();
            glm::vec3 otherBoxRotation = otherModel->GetRotation();// +otherModel->GetRotationOffset();
            glm::vec3 otherBoxSize = glm::vec3((otherModel->mModel->mModel->get_width() * otherModel->GetScale().x), (otherModel->mModel->mModel->get_height() * otherModel->GetScale().y), (otherModel->mModel->mModel->get_length() * otherModel->GetScale().z));

            const std::vector<Renderer::Model::Face>& facesA = GetTriangles(otherBoxPos, otherBoxRotation, otherBoxSize);
            const std::vector<Renderer::Model::Face>& facesB = otherModel->GetTriangles(thisBoxPos, thisBoxRotation, thisBoxSize);

            // Store all collision data
            std::vector<glm::vec3> contactPoints;
            std::vector<float> penetrationDepths;
            std::vector<glm::vec3> contactNormals;

            for (const auto& faceA : facesA)
            {
                glm::vec3 A0 = glm::vec3(modelMatrix * glm::vec4(faceA.a.position, 1.0f));
                glm::vec3 A1 = glm::vec3(modelMatrix * glm::vec4(faceA.b.position, 1.0f));
                glm::vec3 A2 = glm::vec3(modelMatrix * glm::vec4(faceA.c.position, 1.0f));

                for (const auto& faceB : facesB)
                {
                    glm::vec3 B0 = glm::vec3(otherModelMatrix * glm::vec4(faceB.a.position, 1.0f));
                    glm::vec3 B1 = glm::vec3(otherModelMatrix * glm::vec4(faceB.b.position, 1.0f));
                    glm::vec3 B2 = glm::vec3(otherModelMatrix * glm::vec4(faceB.c.position, 1.0f));

                    if (Maths::tri_tri_overlap_test_3d(glm::value_ptr(A0), glm::value_ptr(A1), glm::value_ptr(A2),
                        glm::value_ptr(B0), glm::value_ptr(B1), glm::value_ptr(B2)))
                    {
                        // Calculate an improved collision point.
                        glm::vec3 collisionPoint = Maths::CalculateCollisionPoint(A0, A1, A2, B0, B1, B2);

                        // Compute face normal from triangle A.
                        glm::vec3 normalThis = glm::normalize(glm::cross(A1 - A0, A2 - A0));

                        // The penetration depth is the overlap between the two projection intervals.
                        float penetrationDepth = Maths::CalculatePenetrationDepth(A0, A1, A2, B0, B1, B2);

                        // Store this collision information
                        contactPoints.push_back(collisionPoint);

                        if (penetrationDepth < 1)
                        {
                            penetrationDepths.push_back(penetrationDepth);
                        }
                        else
                        {
                            std::cout << "Penetration depth not included, was " << penetrationDepth << std::endl;
                        }

                        contactNormals.push_back(normalThis);
                    }
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

                // Find the deepest penetration depth
                float maxPenetrationDepth = -1.f;
                size_t maxIndex = 0;
                for (size_t i = 0; i < penetrationDepths.size(); ++i)
                {
                    if (penetrationDepths[i] > maxPenetrationDepth)
                    {
                        maxPenetrationDepth = penetrationDepths[i];
                        maxIndex = i;
                    }
                }

                // Compute a weighted average normal
                glm::vec3 weightedNormal(0.0f);
                for (size_t i = 0; i < contactNormals.size(); ++i)
                {
                    weightedNormal += contactNormals[i] * penetrationDepths[i]; // Weight by depth
                }
                weightedNormal = glm::normalize(weightedNormal);

                // Assign final values
                _collisionPoint = averagedContactPoint;
                _penetrationDepth = maxPenetrationDepth;
                _normal = -weightedNormal;

                return true;
            }
        }

        return false;
    }

    bool ModelCollider::RayCollision(const Ray& _ray, RaycastHit& _outHit)
    {
        // Compute ray's world-space origin.
        glm::vec3 rayOrigin = _ray.origin;
        glm::vec3 rayDirection = glm::normalize(_ray.direction);
		float rayLength = _ray.length;

        // Build the model's world transformation matrix.
        glm::vec3 modelPos = GetPosition() + GetPositionOffset();
        glm::vec3 modelScale = GetScale();
        glm::vec3 modelRotation = GetRotation() + GetRotationOffset();
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, modelPos);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, modelScale);

        // Compute the ray's endpoint
        glm::vec3 rayEnd = rayOrigin + rayDirection * rayLength;

        // Compute min and max of the ray's endpoints.
        glm::vec3 bbMin = glm::min(rayOrigin, rayEnd);
        glm::vec3 bbMax = glm::max(rayOrigin, rayEnd);

        // Calculate the center and size (extents) of the AABB.
        glm::vec3 bbCenter = (bbMin + bbMax) * 0.5f;
        glm::vec3 bbSize = bbMax - bbMin;

        // Retrieve the triangles from the model that lie within the ray's AABB.
        std::vector<Renderer::Model::Face> faces = GetTriangles(bbCenter, glm::vec3(0), bbSize);

        bool hit = false;
        float closestT = rayLength;
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
                if (t >= 0.0f && t < closestT && t <= rayLength)
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
			_outHit.point = hitPoint;
			_outHit.normal = hitNormal;
			_outHit.distance = closestT;
			_outHit.hitEntity = GetEntity();
			_outHit.hit = true;

            return true;
        }

		return false;
    }

    float TetrahedronVolume(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        return glm::dot(a, glm::cross(b, c)) / 6.0f;
    }

    glm::mat3 ModelCollider::UpdateInertiaTensor(float _mass)
    {
        // Accumulators for volume, center of mass, and inertia at the origin.
        float totalVolume = 0.0f;
        glm::vec3 totalCOM(0.0f);
        glm::mat3 I_origin(0.0f);

        std::vector<Renderer::Model::Face> faces = GetModel()->mModel->GetFaces();

        // Iterate over each triangle face and treat it as forming a tetrahedron with the origin.
        for (const auto& tri : faces)
        {
            glm::vec3 v0 = tri.a.position;
            glm::vec3 v1 = tri.b.position;
            glm::vec3 v2 = tri.c.position;

            // Compute the signed volume of the tetrahedron.
            float vol = TetrahedronVolume(v0, v1, v2);
            totalVolume += vol;

            // Tetrahedron center-of-mass (with vertices 0, v0, v1, v2)
            glm::vec3 tetCOM = (glm::vec3(0.0f) + v0 + v1 + v2) / 4.0f;
            totalCOM += vol * tetCOM;

            // Compute approximate inertia integrals for the tetrahedron relative to the origin.
            // Using an integration factor of (vol / 10) with sums over squared terms.
            float Ixx = vol / 10.0f * (
                v0.y * v0.y + v1.y * v1.y + v2.y * v2.y +
                v0.y * v1.y + v0.y * v2.y + v1.y * v2.y +
                v0.z * v0.z + v1.z * v1.z + v2.z * v2.z +
                v0.z * v1.z + v0.z * v2.z + v1.z * v2.z
                );
            float Iyy = vol / 10.0f * (
                v0.x * v0.x + v1.x * v1.x + v2.x * v2.x +
                v0.x * v1.x + v0.x * v2.x + v1.x * v2.x +
                v0.z * v0.z + v1.z * v1.z + v2.z * v2.z +
                v0.z * v1.z + v0.z * v2.z + v1.z * v2.z
                );
            float Izz = vol / 10.0f * (
                v0.x * v0.x + v1.x * v1.x + v2.x * v2.x +
                v0.x * v1.x + v0.x * v2.x + v1.x * v2.x +
                v0.y * v0.y + v1.y * v1.y + v2.y * v2.y +
                v0.y * v1.y + v0.y * v2.y + v1.y * v2.y
                );

            glm::mat3 I_tet(0.0f);
            I_tet[0][0] = Ixx;
            I_tet[1][1] = Iyy;
            I_tet[2][2] = Izz;

            // Sum the inertia contribution weighted by volume.
            I_origin += I_tet;
        }

        // Avoid division by zero if the volume is near zero.
        if (fabs(totalVolume) < 1e-6f)
            return glm::mat3(1.0f);

        glm::mat3 I_com = I_origin;

        // Scale the inertia tensor from "volume mass" to the actual mass.
        // (totalVolume here is proportional to mass if density is 1; scale by _mass / totalVolume)
        glm::mat3 inertiaTensor = I_com * (_mass / totalVolume);

        //return glm::mat3((2.0f / 5.0f) * _mass * mModel->mModel->get_length() * mModel->mModel->get_width());
        return inertiaTensor;
    }


    // --- BVH Building ---

    // Recursively builds a BVH node from a set of faces.
    std::unique_ptr<ModelCollider::BVHNode> ModelCollider::BuildBVH(const std::vector<Renderer::Model::Face>& faces, unsigned int leafThreshold)
    {
        std::unique_ptr<BVHNode> node = std::make_unique<BVHNode>();

        // Compute the AABB that contains all triangles in this node.
        glm::vec3 aabbMin(FLT_MAX);
        glm::vec3 aabbMax(-FLT_MAX);

        for (const auto& face : faces)
        {
            // Use each vertex position from the face (in model local space)
            aabbMin = glm::min(aabbMin, face.a.position);
            aabbMin = glm::min(aabbMin, face.b.position);
            aabbMin = glm::min(aabbMin, face.c.position);

            aabbMax = glm::max(aabbMax, face.a.position);
            aabbMax = glm::max(aabbMax, face.b.position);
            aabbMax = glm::max(aabbMax, face.c.position);
        }
        node->aabbMin = aabbMin;
        node->aabbMax = aabbMax;

        // If the number of faces is small enough, make this a leaf.
        if (faces.size() <= leafThreshold)
        {
            node->triangles = faces;
            return node;
        }

        // Otherwise, choose the axis along which the AABB is widest.
        glm::vec3 extent = aabbMax - aabbMin;
        int axis = 0;
        if (extent.y > extent.x && extent.y > extent.z)
            axis = 1;
        else if (extent.z > extent.x && extent.z > extent.y)
            axis = 2;

        // Copy and sort the faces by the centroid along the chosen axis.
        std::vector<Renderer::Model::Face> sortedFaces = faces;
        std::sort(sortedFaces.begin(), sortedFaces.end(),
            [axis](const Renderer::Model::Face& f1, const Renderer::Model::Face& f2)
            {
                glm::vec3 centroid1 = (f1.a.position + f1.b.position + f1.c.position) / 3.0f;
                glm::vec3 centroid2 = (f2.a.position + f2.b.position + f2.c.position) / 3.0f;
                return centroid1[axis] < centroid2[axis];
            });

        size_t mid = sortedFaces.size() / 2;
        std::vector<Renderer::Model::Face> leftFaces(sortedFaces.begin(), sortedFaces.begin() + mid);
        std::vector<Renderer::Model::Face> rightFaces(sortedFaces.begin() + mid, sortedFaces.end());

        // Recursively build child nodes.
        node->left = BuildBVH(leftFaces, leafThreshold);
        node->right = BuildBVH(rightFaces, leafThreshold);

        return node;
    }


    // --- BVH Query ---
    // Recursively traverses the BVH and adds any triangles in nodes whose AABB
    // overlaps the query AABB.
    void ModelCollider::QueryBVH(const BVHNode* node, const glm::vec3& queryMin, const glm::vec3& queryMax, std::vector<Renderer::Model::Face>& outTriangles)
    {
        if (!node)
            return;

        // Check for overlap between node's AABB and the query AABB.
        if (node->aabbMax.x < queryMin.x || node->aabbMin.x > queryMax.x ||
            node->aabbMax.y < queryMin.y || node->aabbMin.y > queryMax.y ||
            node->aabbMax.z < queryMin.z || node->aabbMin.z > queryMax.z)
        {
            return; // No overlap.
        }

        // If this is a leaf node, add all its triangles.
        if (!node->left && !node->right)
        {
            outTriangles.insert(outTriangles.end(), node->triangles.begin(), node->triangles.end());
            return;
        }

        // Otherwise, query both children.
        if (node->left)
            QueryBVH(node->left.get(), queryMin, queryMax, outTriangles);
        if (node->right)
            QueryBVH(node->right.get(), queryMin, queryMax, outTriangles);
    }

    // --- GetTriangles using BVH ---

    std::vector<Renderer::Model::Face> ModelCollider::GetTriangles(const glm::vec3& boxPos, const glm::vec3& boxRotation, const glm::vec3& boxSize)
    {
        std::vector<Renderer::Model::Face> result;
        if (mModel == nullptr)
            return result;

        // (Re)build the BVH if it hasn?t been built yet or if the leaf threshold has changed.
        if (!mBVHRoot)
        {
            const std::vector<Renderer::Model::Face>& faces = mModel->mModel->GetFaces();
            mBVHRoot = BuildBVH(faces, mBVHLeafThreshold);
        }

        // Compute the world transformation for this model.
        glm::vec3 modelPos = GetPosition() + GetPositionOffset();
        glm::vec3 modelScale = GetScale();
        glm::vec3 modelRotation = GetRotation();// +GetRotationOffset();

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, modelPos);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, modelScale);

        // Transform the box parameters (which are defined in world space) into the model's local space.
        // First, compute the inverse model matrix.
        glm::mat4 invModelMatrix = glm::inverse(modelMatrix);

        // Compute the eight corners of the box in world space.
        glm::vec3 halfSize = boxSize * 0.5f;
        std::vector<glm::vec3> corners;
        corners.reserve(8);
        for (int x = -1; x <= 1; x += 2)
        {
            for (int y = -1; y <= 1; y += 2)
            {
                for (int z = -1; z <= 1; z += 2)
                {
                    corners.push_back(glm::vec3(x * halfSize.x, y * halfSize.y, z * halfSize.z));
                }
            }
        }

        // Build the box's rotation matrix (from its Euler angles).
        glm::mat4 boxRotMatrix = glm::mat4(1.0f);
        boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.x), glm::vec3(1, 0, 0));
        boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.y), glm::vec3(0, 1, 0));
        boxRotMatrix = glm::rotate(boxRotMatrix, glm::radians(boxRotation.z), glm::vec3(0, 0, 1));

        // Transform each corner: first rotate, then translate, then bring into model space.
        for (auto& corner : corners)
        {
            // Apply the box?s rotation and translation.
            corner = glm::vec3(boxRotMatrix * glm::vec4(corner, 1.0f)) + boxPos;
            // Transform from world space into model local space.
            corner = glm::vec3(invModelMatrix * glm::vec4(corner, 1.0f));
        }

        // Compute an axis?aligned bounding box (AABB) from the transformed corners.
        glm::vec3 queryMin = corners[0];
        glm::vec3 queryMax = corners[0];
        for (size_t i = 1; i < corners.size(); ++i)
        {
            queryMin = glm::min(queryMin, corners[i]);
            queryMax = glm::max(queryMax, corners[i]);
        }

        // Query the BVH for triangles that might intersect the box.
        QueryBVH(mBVHRoot.get(), queryMin, queryMax, result);

        return result;
    }
}