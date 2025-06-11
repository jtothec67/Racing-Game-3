#pragma once

#include "Collider.h"

#ifdef _DEBUG
#include "Renderer/Model.h"
#endif


namespace JamesEngine
{

	class BoxCollider : public Collider
	{
	public:
#ifdef _DEBUG
		void OnGUI();
#endif

		bool IsColliding(std::shared_ptr<Collider> _other, glm::vec3& _collisionPoint, glm::vec3& _normal, float& _penetrationDepth);
		bool RayCollision(const Ray& _ray, RaycastHit& _outHit) { return false; }

		glm::mat3 UpdateInertiaTensor(float _mass);

		void SetSize(glm::vec3 _size) { mSize = _size; }
		glm::vec3 GetSize() { return mSize; }

	private:
		glm::vec3 mSize{ 1 };

#ifdef _DEBUG
		std::shared_ptr<Renderer::Model> mModel = std::make_shared<Renderer::Model>("../assets/shapes/cube.obj");
#endif
	};

}