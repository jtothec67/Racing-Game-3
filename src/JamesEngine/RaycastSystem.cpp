#include "RaycastSystem.h"

#include "Core.h"
#include "Entity.h"
#include "Collider.h"

namespace JamesEngine
{
	
	RaycastSystem::RaycastSystem(std::shared_ptr<Core> _core)
	{
		mCore = _core;
	}

	bool RaycastSystem::Raycast(const Ray& _ray, RaycastHit& _outHit)
	{
		if (_ray.length <= 0.0f || _ray.direction == glm::vec3(0.0f))
		{
			_outHit.hit = false;
			return false;
		}

		// Get all colliders in the scene
		if (mCollidersInScene.empty())
			mCore.lock()->FindComponents(mCollidersInScene);

		bool hitSomething = false;
		float closestDist = _ray.length;
		RaycastHit tempHit;

		for (auto& collider : mCollidersInScene)
		{
			if (collider->RayCollision(_ray, tempHit))
			{
				if (tempHit.distance < closestDist)
				{
					closestDist = tempHit.distance;
					_outHit = tempHit;
					hitSomething = true;
				}
			}
		}

		return hitSomething;
	}

}