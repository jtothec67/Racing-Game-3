#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace JamesEngine
{

	class Mouse
	{
	public:
		bool IsButton(int _button);
		bool IsButtonDown(int _button);
		bool IsButtonUp(int _button);

		int GetXPosition() { return mXpos; }
		int GetYPosition() { return mYpos; }

		glm::ivec2 GetPosition() { return glm::vec2(mXpos, mYpos); }
		glm::ivec2 GetDelta() { return mDelta; }

	private:
		friend class Input;

		void Update();

		void ButtonBeenReleased(int _button)
		{
			for (int i = 0; i < mButtons.size(); ++i)
			{
				if (mButtons[i] == _button)
				{
					mButtons.erase(mButtons.begin() + i);
					--i;
				}
			}
		}

		int mXpos = 0;
		int mYpos = 0;

		glm::ivec2 mDelta{ 1 };

		int mOldXpos = 0;
		int mOldYpos = 0;

		std::vector<int> mButtons;
		std::vector<int> mButtonsDown;
		std::vector<int> mButtonsUp;
	};

}