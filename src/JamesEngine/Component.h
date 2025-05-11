#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace JamesEngine
{

	class Entity;
	class Input;
	class Keyboard;
	class Mouse;
	class GUI;
	class Transform;
	class Core;

	/**
	 * @class Component
	 * @brief Base class for all components.
	 */
	class Component
	{
	public:
		std::shared_ptr<Entity> GetEntity();
		std::shared_ptr<Core> GetCore();
		std::shared_ptr<Input> GetInput();
		std::shared_ptr<Keyboard> GetKeyboard();
		std::shared_ptr<Mouse> GetMouse();
		std::shared_ptr<GUI> GetGUI();
		std::shared_ptr<Transform> GetTransform();

		glm::vec3 GetPosition();
		glm::vec3 GetLocalPosition();
		void SetPosition(glm::vec3 _position);
		glm::vec3 GetRotation();
		void SetRotation(glm::vec3 _rotation);
		glm::quat GetWorldRotation();
		glm::vec3 GetWorldRotationEuler();
		glm::quat GetQuaternion();
		void SetQuaternion(const glm::quat& _quat);
		glm::vec3 GetScale();
		void SetScale(glm::vec3 _scale);
		void Move(glm::vec3 _amount);
		void Rotate(glm::vec3 _amount);

		/**
		 * @brief Called when the component is initialized.
		 */
		virtual void OnInitialize() { }
		/**
		 * @brief Called every tick.
		 */
		virtual void OnTick() { }
		/**
		 * @brief Called 200 times a second.
		 */
		virtual void OnFixedTick() {}
		/**
		 * @brief Called after OnFixedTick().
		 */
		virtual void OnLateFixedTick() {}
		/**
		 * @brief Called after OnTick().
		 */
		virtual void OnRender() { }
		/**
		 * @brief Called After OnRender().
		 */
		virtual void OnGUI() { }
		/**
		 * @brief Called when the entity the component is attatched to is destroyed.
		 */
		virtual void OnDestroy() { }
		/**
		 * @brief Called on both colliders when two colliders collide, as long as at least one has a rigid body.
		 * @param _tag The tag of the entity it collided with.
		 */
		virtual void OnCollision(std::shared_ptr<Entity> _collidedEntity) { }
		/**
		 * @brief Called on the first frame entity has been created.
		 */
		virtual void OnAlive() {}

	private:
		friend class JamesEngine::Entity;

		std::weak_ptr<Entity> mEntity;

		void Tick();
		void FixedTick();
		void LateFixedTick();
		void Render();
		void GUI();
		void Destroy();
		void Alive();
	};
}