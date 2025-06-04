#pragma once

#include <iostream>

#include <string>
#include <memory>
#include <vector>

namespace JamesEngine
{

	class Core;
	class Component;

	/**
	 * @class Entity
	 * @brief Used to add components to.
	 */
	class Entity
	{
	public:
		/**
		 * @brief Gets the core of the engine.
		 * @return A shared pointer to the core.
		 */
		std::shared_ptr<Core> GetCore();

		std::string GetTag() { return mTag; }
		void SetTag(std::string _tag) { mTag = _tag; }

		/**
		 * @brief Adds a component to the entity. Has ability to pass arguments to the component's constructor (not used).
		 * @tparam T The type of the component.
		 * @tparam Args The types of the arguments to pass to the component's constructor.
		 * @param args The arguments to pass to the component's constructor.
		 * @return A shared pointer to the added component.
		 */
		template<typename T, typename... Args>
		std::shared_ptr<T> AddComponent(Args&&... args)
		{
			std::shared_ptr<T> rtn = std::make_shared<T>(std::forward<Args>(args)...);

			rtn->mEntity = mSelf;
			rtn->OnInitialize();
			mComponents.push_back(rtn);

			return rtn;
		}

		/**
		 * @brief Gets a component of the specified type.
		 * @tparam T The type of the component.
		 * @return A shared pointer to the component, or nullptr if not found.
		 */
		template <typename T>
		std::shared_ptr<T> GetComponent()
		{
			for (size_t i = 0; i < mComponents.size(); ++i)
			{
				std::shared_ptr<T> rtn = std::dynamic_pointer_cast<T>(mComponents[i]);
				if (rtn)
				{
					return rtn;
				}
			}

			return nullptr;
		}

		/**
		 * @brief Calls OnDestroy on all of the entities component, then removes the entity from core for next frame. Will only call OnDestroy once, even if Destroy is called twice.
		 */
		void Destroy();

	private:
		friend class Core;
		friend class Rigidbody;

		std::weak_ptr<Core> mCore;
		std::weak_ptr<Entity> mSelf;

		std::vector<std::shared_ptr<Component>> mComponents;

		std::string mTag = "Default";

		bool mAlive = true;

		bool mJustCreated = true;

		void OnTick();
		void OnEarlyFixedTick();
		void OnFixedTick();
		void OnLateFixedTick();
		void OnRender();
		void OnGUI();
	};

}