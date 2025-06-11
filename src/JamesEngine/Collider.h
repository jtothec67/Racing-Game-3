#pragma once

#include "Component.h"
#include "RaycastSystem.h"

#ifdef _DEBUG
#include "Renderer/Shader.h"
#endif


namespace JamesEngine
{

	class Collider : public Component
	{
	public:
#ifdef _DEBUG
		virtual void OnGUI() {}
#endif

		virtual bool IsColliding(std::shared_ptr<Collider> _other, glm::vec3& _collisionPoint, glm::vec3& _normal, float& _penetrationDepth) = 0;
		virtual bool RayCollision(const Ray& _ray, RaycastHit& _outHit) = 0;

		virtual glm::mat3 UpdateInertiaTensor(float _mass) = 0;

		void SetPositionOffset(glm::vec3 _offset) { mPositionOffset = _offset; }
		glm::vec3 GetPositionOffset() { return mPositionOffset; }

		void SetRotationOffset(glm::vec3 _rotation) { mRotationOffset = _rotation; }
		glm::vec3 GetRotationOffset() { return mRotationOffset; }

		void SetDebugVisual(bool _value) { mDebugVisual = _value; }

		void IsTrigger(bool _value) { mIsTrigger = _value; }
		bool IsTrigger() { return mIsTrigger; }

	protected:
		friend class BoxCollider;
		friend class SphereCollider;

		glm::vec3 mPositionOffset{ 0 };
		glm::vec3 mRotationOffset{ 0 };

		bool mIsTrigger = false;

		bool mDebugVisual = true;

#ifdef _DEBUG
		std::shared_ptr<Renderer::Shader> mShader = std::make_shared<Renderer::Shader>("../assets/shaders/OutlineShader.vert", "../assets/shaders/OutlineShader.frag");
#endif
	};

}