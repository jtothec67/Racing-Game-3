#include "Component.h"

#include <glm/glm.hpp>

namespace JamesEngine
{

	class Camera : public Component
	{
	public:
		glm::mat4 GetViewMatrix();
		glm::mat4 GetProjectionMatrix();

		void SetFov(float _fov) { mFov = _fov; }
		void SetNearClip(float _nearClip) { mNearClip = _nearClip; }
		void SetFarClip(float _farClip) { mFarClip = _farClip; }

		void SetPriority(float _priority) { mPriority = _priority; }
		float GetPriority() { return mPriority; }

	private:
		float mFov = 60.f;
		float mNearClip = 0.1f;
		float mFarClip = 4000.f;

		// Higher priotity, more important
		float mPriority = 1.f;
	};

}