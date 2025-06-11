#pragma once

#include "Collider.h"

#ifdef _DEBUG
#include "Renderer/Model.h"
#endif


namespace JamesEngine
{

	class RayCollider : public Collider
	{
	public:
#ifdef _DEBUG
		void OnGUI();
#endif

		bool IsColliding(std::shared_ptr<Collider> _other, glm::vec3& _collisionPoint, glm::vec3& _normal, float& _penetrationDepth);
		bool RayCollision(const Ray& _ray, RaycastHit& _outHit) { return false; }

		glm::mat3 UpdateInertiaTensor(float _mass) { return glm::mat3(0.1); }

		void SetPositionOffset(glm::vec3 _positionOffset) { mPositionOffset = _positionOffset; }
		glm::vec3 GetPositionOffset() { return mPositionOffset; }

		void SetDirection(glm::vec3 _direction) { mDirection = _direction; }
		glm::vec3 GetDirection() { return mDirection; }

		void SetLength(float _length) { mLength = _length; }
		float GetLength() { return mLength; }

	private:
		glm::vec3 mPositionOffset{ 0 };
		glm::vec3 mDirection{ 0, -1, 0 };
		float mLength = 5;

		float mSteepnessThreshold = 0.5f;
		float mMinPenetrationPercentage = 0.2f;

#ifdef _DEBUG
		std::shared_ptr<Renderer::Model> mModel = std::make_shared<Renderer::Model>("../assets/shapes/cylinder.obj");
#endif
	};

}