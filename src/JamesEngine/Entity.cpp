#include "Entity.h"

#include "Component.h"
#include "Core.h"

namespace JamesEngine
{

	std::shared_ptr<Core> Entity::GetCore()
	{
		return mCore.lock();
	}

	void Entity::OnTick()
	{
		if (mJustCreated)
		{
			mJustCreated = false;
			for (size_t ci = 0; ci < mComponents.size(); ++ci)
			{
				mComponents.at(ci)->Alive();
			}
		}

		for (size_t ci = 0; ci < mComponents.size(); ++ci)
		{
			mComponents.at(ci)->Tick();
		}
	}

	void Entity::OnEarlyFixedTick()
	{
		for (size_t ci = 0; ci < mComponents.size(); ++ci)
		{
			mComponents.at(ci)->EarlyFixedTick();
		}
	}

	void Entity::OnFixedTick()
	{
		for (size_t ci = 0; ci < mComponents.size(); ++ci)
		{
			mComponents.at(ci)->FixedTick();
		}
	}

	void Entity::OnLateFixedTick()
	{
		for (size_t ci = 0; ci < mComponents.size(); ++ci)
		{
			mComponents.at(ci)->LateFixedTick();
		}
	}

	void Entity::OnRender()
	{
		for (size_t ci = 0; ci < mComponents.size(); ++ci)
		{
			mComponents.at(ci)->Render();
		}
	}

	void Entity::OnGUI()
	{
		for (size_t ci = 0; ci < mComponents.size(); ++ci)
		{
			mComponents.at(ci)->GUI();
		}
	}

	void Entity::Destroy()
	{
		if (mAlive)
		{
			mAlive = false;

			for (size_t ci = 0; ci < mComponents.size(); ci++)
			{
				mComponents.at(ci)->Destroy();
			}
		}
	}

}