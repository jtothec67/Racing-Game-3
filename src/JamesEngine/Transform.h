#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace JamesEngine
{
	class Transform : public Component
	{
	public:
        glm::mat4 GetModel();

        void SetParent(std::shared_ptr<Entity> _parent) { mParent = _parent; }

        void SetPosition(glm::vec3 _position) { mPosition = _position; }
		glm::vec3 GetLocalPosition() { return mPosition; }
        glm::vec3 GetPosition();

        void SetRotation(glm::vec3 _rotation)
        {
            mEulerRotation = _rotation;
            mRotation = glm::quat(glm::radians(_rotation));
        }
        glm::vec3 GetRotation() { return mEulerRotation; }

        glm::quat GetQuaternion() const { return mRotation; }
        void SetQuaternion(const glm::quat& quat)
        {
            mRotation = quat;
            glm::vec3 euler = glm::degrees(glm::eulerAngles(quat));
            euler.x = -euler.x;
            mEulerRotation = euler;
        }

        void SetScale(glm::vec3 _scale) { mScale = _scale; }
        glm::vec3 GetScale();

        glm::vec3 GetForward();
        glm::vec3 GetRight();
        glm::vec3 GetUp();

        void Move(glm::vec3 _amount) { mPosition += _amount; }
        void Rotate(glm::vec3 _rotation)
        {
            mEulerRotation += _rotation;
            mRotation = glm::quat(glm::radians(mEulerRotation));
        }

        glm::quat GetWorldRotation();
		glm::vec3 GetWorldRotationEuler();

    private:
        glm::vec3 mPosition{ 0.f };
        glm::vec3 mEulerRotation{ 0.f };
        glm::quat mRotation{ 1.f, 0.f, 0.f, 0.f };
        glm::vec3 mScale{ 1.f };

        std::weak_ptr<Entity> mParent;
	};
}