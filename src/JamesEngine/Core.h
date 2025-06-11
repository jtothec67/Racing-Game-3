#pragma once

#include "Window.h"
#include "Audio.h"
#include "GUI.h"
#include "LightManager.h"
#include "RaycastSystem.h"

#include <memory>
#include <vector>

namespace JamesEngine
{

	class Input;
	class Entity;
	class Resources;
	class Camera;
	class Skybox;

	/**
	 * @class Core
	 * @brief The main class for initializing and running the engine.
	 */
	class Core
	{
	public:
		/**
		 * @brief Initializes the Core with a given window size.
		 * @param _windowSize The initial size of the window.
		 * @return A shared pointer to the initialized Core.
		 */
		static std::shared_ptr<Core> Initialize(glm::ivec2 _windowSize);

		/**
		 * @brief Runs the main loop of the engine.
		 */
		void Run();
		/**
		 * @brief Stops the execution of the engine.
		 */
		void End() { mIsRunning = false; }

		std::shared_ptr<Window> GetWindow() const { return mWindow; }
		std::shared_ptr<Input> GetInput() const { return mInput; }
		std::shared_ptr<Resources> GetResources() const { return mResources; }
		std::shared_ptr<GUI> GetGUI() const { return mGUI; }
		std::shared_ptr<LightManager> GetLightManager() const { return mLightManager; }
		std::shared_ptr<Skybox> GetSkybox() const { return mSkybox; }
		std::shared_ptr<RaycastSystem> GetRaycastSystem() const { return mRaycastSystem; }

		/**
		 * @brief Adds a new entity to the engine.
		 * @return A shared pointer to the newly added entity.
		 */
		std::shared_ptr<Entity> AddEntity();

		/**
		 * @brief Gets the current camera with the highest priority.
		 * @return A shared pointer to the camera.
		 */
		std::shared_ptr<Camera> GetCamera();

		/**
		 * @brief Destroys all entities in the scene. (Make sure to add entities after or the engine sof tlocks itself.)
		 */
		void DestroyAllEntities();

		/**
		 * @brief Finds all entities with tag _tag in the entities.
		 * @param _tag A tag to identify entities.
		 * @return A vector of shared pointers to the found entities.
		 */
		std::vector<std::shared_ptr<Entity>> GetEntitiesByTag(std::string _tag);

		/**
		* @brief Finds the first entity with tag _tag in the entities. Useful if you know there is only one.
		* @param _tag A tag to identify entities.
		* @return A shared pointer to the found entity, or nullptr if not found.
		*/
		std::shared_ptr<Entity> GetEntityByTag(std::string _tag);

		/**
		 * @brief Finds all components of type T in the entities.
		 * @tparam T The type of components to find.
		 * @param _out A vector to store the found components.
		 */
		template <typename T>
		void FindComponents(std::vector<std::shared_ptr<T>>& _out)
		{
			for (size_t ei = 0; ei < mEntities.size(); ++ei)
			{
				std::shared_ptr<Entity> e = mEntities.at(ei);
				for (size_t ci = 0; ci < e->mComponents.size(); ++ci)
				{
					std::shared_ptr<Component> c = e->mComponents.at(ci);
					std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(c);

					if (t)
					{
						_out.push_back(t);
					}
				}
			}
		}

		/**
		 * @brief Finds the first component of type T in the entities. Useful if you know there is only one.
		 * @tparam T The type of component to find.
		 * @return A shared pointer to the found component, or nullptr if not found.
		 */
		template <typename T>
		std::shared_ptr<T> FindComponent()
		{
			for (size_t ei = 0; ei < mEntities.size(); ++ei)
			{
				std::shared_ptr<Entity> e = mEntities.at(ei);
				for (size_t ci = 0; ci < e->mComponents.size(); ++ci)
				{
					std::shared_ptr<Component> c = e->mComponents.at(ci);
					std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(c);

					if (t)
					{
						return t;
					}
				}
			}

			return nullptr;
		}

		/**
		 * @brief Gets the delta time.
		 * @return The time taken for the last frame to update and draw.
		 */
		float DeltaTime() { return mDeltaTime; }

		float GetTimeScale() { return mTimeScale; }
		void SetTimeScale(float _timeScale) { mTimeScale = _timeScale; }

		float FixedDeltaTime() { return mFixedDeltaTime; }

	private:
		std::shared_ptr<Window> mWindow;
		std::shared_ptr<Audio> mAudio;
		std::shared_ptr<Input> mInput;
		std::shared_ptr<GUI> mGUI;
		std::shared_ptr<LightManager> mLightManager;
		std::shared_ptr<Skybox> mSkybox;
		std::shared_ptr<RaycastSystem> mRaycastSystem;
		std::shared_ptr<Resources> mResources;
		std::vector<std::shared_ptr<Entity>> mEntities;
		std::weak_ptr<Core> mSelf;

		bool mIsRunning = true;

		float mDeltaTime = 0.0f;

		float mFixedDeltaTime = 0.01f; // 100 fps
		float mFixedTimeAccumulator = 0.0f;

		float mTimeScale = 1.f;

		// Used when loading scenes to ensure first frames don't have a large delta time
		bool mDeltaTimeZero = true;
		int mDeltaTimeZeroCounter = 0;
		int mNumDeltaTimeZeros = 2;
	};

}