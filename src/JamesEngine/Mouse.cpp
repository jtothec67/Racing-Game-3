#include "Mouse.h"

#include <iostream>

namespace JamesEngine
{

	void Mouse::Update()
	{
		mButtonsDown.clear();
		mButtonsUp.clear();

		mDelta = glm::ivec2(mOldXpos - mXpos, mOldYpos - mYpos);

		mOldXpos = mXpos;
		mOldYpos = mYpos;
	}

	bool Mouse::IsButton(int _Button)
	{
		for (int i = 0; i < mButtons.size(); ++i)
		{
			if (mButtons[i] == _Button)
			{
				return true;
			}
		}
		return false;
	}

	bool Mouse::IsButtonDown(int _Button)
	{
		for (int i = 0; i < mButtonsDown.size(); ++i)
		{
			if (mButtonsDown[i] == _Button)
			{
				return true;
			}
		}
		return false;
	}

	bool Mouse::IsButtonUp(int _Button)
	{
		for (int i = 0; i < mButtonsUp.size(); ++i)
		{
			if (mButtonsUp[i] == _Button)
			{
				return true;
			}
		}
		return false;
	}

}