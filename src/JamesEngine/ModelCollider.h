#pragma once

#include "Collider.h"
#include "Model.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace JamesEngine
{
    // Forward declaration for BoxCollider.
    class BoxCollider;

    class ModelCollider : public Collider
    {
    public:
#ifdef _DEBUG
        void OnGUI();
#endif
		void OnAlive();

        bool IsColliding(std::shared_ptr<Collider> _other, glm::vec3& _collisionPoint, glm::vec3& _normal, float& _penetrationDepth);

        glm::mat3 UpdateInertiaTensor(float _mass);

        void SetModel(std::shared_ptr<Model> _model) { mModel = _model; }
        std::shared_ptr<Model> GetModel() { return mModel; }

        // GetTriangles returns the candidate triangles (in model space)
        // that lie within (or near) the provided BoxCollider. The _leafThreshold
        // parameter controls how many triangles are allowed per leaf node.
        std::vector<Renderer::Model::Face> GetTriangles(const glm::vec3& boxPos, const glm::vec3& boxRotation, const glm::vec3& boxSize);

    private:
        std::shared_ptr<Model> mModel = nullptr;

        // --- BVH Data Structure ---
        // Each node holds an axis-aligned bounding box (AABB) and either a list of triangles (if a leaf)
        // or pointers to two children.
        struct BVHNode
        {
            glm::vec3 aabbMin;
            glm::vec3 aabbMax;
            std::vector<Renderer::Model::Face> triangles; // non-empty only in leaf nodes
            std::unique_ptr<BVHNode> left;
            std::unique_ptr<BVHNode> right;
        };

        // Cached BVH built from the model’s triangles (in local space)
        std::unique_ptr<BVHNode> mBVHRoot = nullptr;
        // The leaf–threshold used for the current BVH
        unsigned int mBVHLeafThreshold = 2;

        // Helper functions to build and query the BVH.
        std::unique_ptr<BVHNode> BuildBVH(const std::vector<Renderer::Model::Face>& faces, unsigned int leafThreshold);
        void QueryBVH(const BVHNode* node, const glm::vec3& queryMin, const glm::vec3& queryMax, std::vector<Renderer::Model::Face>& outTriangles);
    };
}
