#pragma once

#include <SDL2/sdl.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>

namespace JamesEngine
{

	class Window
	{
	public:
		Window(int _width, int _height);
		~Window();

		void Update();

		void ClearWindow();

		void SwapWindows() { SDL_GL_SwapWindow(mRaw); }

		void GetWindowSize(int& _width, int& _height) { _width = mWidth; _height = mHeight; }

		void SetClearColour(glm::vec3 _colour);

		void ActivateVSync(bool _activate) { SDL_GL_SetSwapInterval(_activate ? 1 : 0); mVSync = _activate; }

		bool IsVSyncActive() { return mVSync; }

		void HideMouse(bool _hide) { SDL_ShowCursor(_hide ? SDL_DISABLE : SDL_ENABLE); }

		void RelativeMouseMode(bool _relative) { SDL_SetRelativeMouseMode(_relative ? SDL_TRUE : SDL_FALSE); }

		void SetMousePosition(int _x, int _y) { SDL_WarpMouseInWindow(mRaw, _x, _y); }

		void SetWindowTitle(std::string _title) { SDL_SetWindowTitle(mRaw, _title.c_str()); }

	private:
		SDL_Window* mRaw;

		SDL_GLContext mContext;

		int mWidth;
		int mHeight;

		glm::vec3 mClearColour = glm::vec3(1.0f, 0.0f, 0.0f);

		bool mVSync = false;

		Window(const Window& _copy);
		Window& operator*(const Window& _assign);
	};

}