#include "Transform.h"
#include "Entity.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace JamesEngine
{

    glm::vec3 Transform::GetPosition()
    {
        if (std::shared_ptr<Entity> parent = mParent.lock())
        {
            std::shared_ptr<Transform> parentTransform = parent->GetComponent<Transform>();
            glm::vec3 parentPosition = parentTransform->GetPosition();

            glm::mat4 rotationMatrix = glm::toMat4(parentTransform->GetWorldRotation());
            glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(mPosition, 1.0f);

            return glm::vec3(rotatedPosition) + parentPosition;
        }
        return mPosition;
    }

    glm::vec3 Transform::GetScale()
    {
        if (std::shared_ptr<Entity> parent = mParent.lock())
            return mScale * parent->GetComponent<Transform>()->GetScale();
        return mScale;
    }

    glm::mat4 Transform::GetModel()
    {
        glm::mat4 modelMatrix = glm::mat4(1.f);
        modelMatrix = glm::translate(modelMatrix, GetPosition());
        modelMatrix *= glm::toMat4(GetWorldRotation());
        modelMatrix = glm::scale(modelMatrix, GetScale());

        return modelMatrix;
    }

    glm::vec3 Transform::GetForward()
    {
        glm::quat worldRot = GetWorldRotation();
        return glm::normalize(worldRot * glm::vec3(0.f, 0.f, 1.f));
    }

    glm::vec3 Transform::GetRight()
    {
        glm::quat worldRot = GetWorldRotation();
        return glm::normalize(worldRot * glm::vec3(-1.f, 0.f, 0.f));
    }

    glm::vec3 Transform::GetUp()
    {
        glm::quat worldRot = GetWorldRotation();
        return glm::normalize(worldRot * glm::vec3(0.f, 1.f, 0.f));
    }

    glm::quat Transform::GetWorldRotation()
    {
        if (std::shared_ptr<Entity> parent = mParent.lock())
        {
            std::shared_ptr<Transform> parentTransform = parent->GetComponent<Transform>();
            return parentTransform->GetWorldRotation() * mRotation;
        }
        return mRotation;
    }

	glm::vec3 Transform::GetWorldRotationEuler()
	{
		glm::quat worldRot = GetWorldRotation();
		glm::vec3 euler = glm::degrees(glm::eulerAngles(worldRot));
		euler.x = -euler.x;
		return euler;
	}

}