#pragma once

#include "Component.h"

#ifdef _DEBUG
#include "Renderer/Shader.h"
#endif


namespace JamesEngine
{
	class Rigidbody;

	class Suspension : public Component
	{
	public:
#ifdef _DEBUG
		void OnGUI();
#endif

		void OnAlive();
		void OnFixedTick();
		void OnLateFixedTick();
		void OnTick();

		void SetWheel(std::shared_ptr<Entity> _wheel) { mWheel = _wheel; }
		void SetCarBody(std::shared_ptr<Entity> _carBody) { mCarBody = _carBody; }
		void SetAnchorPoint(std::shared_ptr<Entity> _anchorPoint) { mAnchorPoint = _anchorPoint; }

		void SetStiffness(float _stiffness) { mStiffness = _stiffness; }
		void SetDamping(float _damping) { mDamping = _damping; }

		void SetCollision(bool _groundContact) { mGroundContact = _groundContact; }
		bool GetCollision() { return mGroundContact; }

		void SetSteeringAngle(float _steeringAngle) { mSteeringAngle = _steeringAngle; }
		float GetSteeringAngle() { return mSteeringAngle; }

		void SetSuspensionTravel(float _suspensionTravel) { mSuspensionTravel = _suspensionTravel; }
		float GetSuspensionTravel() { return mSuspensionTravel; }

		void SetWheelRadius(float _wheelRadius) { mWheelRadius = _wheelRadius; }
		float GetWheelRadius() { return mWheelRadius; }

		void SetRestLength(float _restLength) { mRestLength = _restLength; }
		float GetRestLength() { return mRestLength; }

		void SetDebugVisual(bool _value) { mDebugVisual = _value; }

		void SetHitDistance(float _hitDistance) { mHitDistance = _hitDistance; }

		float GetForce() { return mForce; }

		void SetSurfaceNormal(glm::vec3 _surfaceNormal) { mSurfaceNormal = _surfaceNormal; }
		glm::vec3 GetSurfaceNormal() { return mSurfaceNormal; }

	private:
		std::shared_ptr<Entity> mWheel;
		std::shared_ptr<Entity> mCarBody;
		std::shared_ptr<Entity> mAnchorPoint;

		std::shared_ptr<Rigidbody> mWheelRb;
		std::shared_ptr<Rigidbody> mCarRb;

		float mStiffness = 500;
		float mDamping = 50;

		bool mGroundContact = false;
		float mHitDistance = 0.0f;

		float mSteeringAngle = 0.0f;

		float mSuspensionTravel = 0.1f;
		float mWheelRadius = 0.34f;

		float  mRestLength = 0.02f;

		glm::vec3 mSurfaceNormal{ 0 };

		float mForce = 0.0f;

		bool mDebugVisual = true;

#ifdef _DEBUG
		std::shared_ptr<Renderer::Shader> mShader = std::make_shared<Renderer::Shader>("../assets/shaders/OutlineShader.vert", "../assets/shaders/OutlineShader.frag");
		std::shared_ptr<Renderer::Model> mModel = std::make_shared<Renderer::Model>("../assets/shapes/spring.obj");
#endif
	};

}