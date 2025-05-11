#pragma once

#include <vector>

namespace JamesEngine
{

	class Keyboard
	{
	public:
		bool IsKey(int _key);
		bool IsKeyDown(int _key);
		bool IsKeyUp(int _key);

	private:
		friend class Input;

		void Update();

		void KeyBeenReleased(int _key)
		{
			for (int i = 0; i < mKeys.size(); ++i)
			{
				if (mKeys[i] == _key)
				{
					mKeys.erase(mKeys.begin() + i);
					--i;
				}
			}
		}

		std::vector<int> mKeys;
		std::vector<int> mKeysDown;
		std::vector<int> mKeysUp;
	};

}