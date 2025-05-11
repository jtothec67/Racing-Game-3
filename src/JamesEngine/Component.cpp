#include "Component.h"
#include "Entity.h"
#include "Transform.h"
#include "Core.h"
#include "Input.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace JamesEngine
{

	std::shared_ptr<Entity> Component::GetEntity()
	{
		return mEntity.lock();
	}

	std::shared_ptr<Core> Component::GetCore()
	{
		return GetEntity()->GetCore();
	}

	std::shared_ptr<Input> Component::GetInput()
	{
		return GetEntity()->GetCore()->GetInput();
	}

	std::shared_ptr<Keyboard> Component::GetKeyboard()
	{
		return GetEntity()->GetCore()->GetInput()->GetKeyboard();
	}

	std::shared_ptr<Mouse> Component::GetMouse()
	{
		return GetEntity()->GetCore()->GetInput()->GetMouse();
	}

	std::shared_ptr<GUI> Component::GetGUI()
	{
		return GetEntity()->GetCore()->GetGUI();
	}

	std::shared_ptr<Transform> Component::GetTransform()
	{
		return GetEntity()->GetComponent<Transform>();
	}

	glm::vec3 Component::GetPosition()
	{
		return GetTransform()->GetPosition();
	}

	glm::vec3 Component::GetLocalPosition()
	{
		return GetTransform()->GetLocalPosition();
	}

	void Component::SetPosition(glm::vec3 _position)
	{
		GetTransform()->SetPosition(_position);
	}

	glm::vec3 Component::GetRotation()
	{
		return GetTransform()->GetRotation();
	}

	void Component::SetRotation(glm::vec3 _rotation)
	{
		GetTransform()->SetRotation(_rotation);
	}

	glm::quat Component::GetWorldRotation()
	{
		return GetTransform()->GetWorldRotation();
	}

	glm::vec3 Component::GetWorldRotationEuler()
	{
		return GetTransform()->GetWorldRotationEuler();
	}

	glm::quat Component::GetQuaternion()
	{
		return GetTransform()->GetQuaternion();
	}

	void Component::SetQuaternion(const glm::quat& _quat)
	{
		GetTransform()->SetQuaternion(_quat);
	}

	glm::vec3 Component::GetScale()
	{
		return GetTransform()->GetScale();
	}

	void Component::SetScale(glm::vec3 _scale)
	{
		GetTransform()->SetScale(_scale);
	}

	void Component::Move(glm::vec3 _amount)
	{
		GetTransform()->Move(_amount);
	}

	void Component::Rotate(glm::vec3 _amount)
	{
		GetTransform()->Rotate(_amount);
	}

	void Component::Tick()
	{
		OnTick();
	}

	void Component::FixedTick()
	{
		OnFixedTick();
	}

	void Component::LateFixedTick()
	{
		OnLateFixedTick();
	}

	void Component::Render()
	{
		OnRender();
	}

	void Component::GUI()
	{
		OnGUI();
	}

	void Component::Destroy()
	{
		OnDestroy();
	}

	void Component::Alive()
	{
		OnAlive();
	}

}