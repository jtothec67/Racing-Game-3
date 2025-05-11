#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Controller.h"

#include <SDL2/sdl.h>
#include <memory>

namespace JamesEngine
{
	class Core;

	class Input
	{
	public:
		Input();

		void Update();

		std::shared_ptr<Keyboard> GetKeyboard() { return mKeyboard; }
		std::shared_ptr<Mouse> GetMouse() { return mMouse; }
		std::shared_ptr<Controller> GetController() { return mController; }

	private:
		friend class Core;

		void HandleInput(const SDL_Event& _event);

		std::shared_ptr<Keyboard> mKeyboard;
		std::shared_ptr<Mouse> mMouse;
		std::shared_ptr<Controller> mController;
	};

}