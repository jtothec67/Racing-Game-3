#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace JamesEngine
{

	class Collider;
	class Entity;
	class Core;

	struct Ray
	{
		glm::vec3 origin{ 0.0f };
		glm::vec3 direction{ 0.0f }; // Direction must be normalised by user
		float length = 0;
	};

	struct RaycastHit
	{
		glm::vec3 point;
		glm::vec3 normal;
		float distance;
		std::shared_ptr<Entity> hitEntity;
		bool hit;
	};

	class RaycastSystem
	{
	public:
		RaycastSystem(std::shared_ptr<Core> _core);
		~RaycastSystem() {}

		bool Raycast(const Ray& _ray, RaycastHit& _outHit);

	private:
		friend class Core;

		void ClearCache()
		{
			mCollidersInScene.clear();
		}

		std::vector<std::shared_ptr<Collider>> mCollidersInScene;

		std::weak_ptr<Core> mCore;
	};

}