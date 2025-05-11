#include "Keyboard.h"

namespace JamesEngine
{

	void Keyboard::Update()
	{
		mKeysDown.clear();
		mKeysUp.clear();
	}

	bool Keyboard::IsKey(int _key)
	{
		for (int i = 0; i < mKeys.size(); ++i)
		{
			if (mKeys[i] == _key)
			{
				return true;
			}
		}
		return false;
	}

	bool Keyboard::IsKeyDown(int _key)
	{
		for (int i = 0; i < mKeysDown.size(); ++i)
		{
			if (mKeysDown[i] == _key)
			{
				return true;
			}
		}
		return false;
	}

	bool Keyboard::IsKeyUp(int _key)
	{
		for (int i = 0; i < mKeysUp.size(); ++i)
		{
			if (mKeysUp[i] == _key)
			{
				return true;
			}
		}
		return false;
	}

}