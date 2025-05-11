#include "Window.h"

#include <iostream>
#include <exception>

namespace JamesEngine
{

	Window::Window(int _width, int _height)
	{
		mWidth = _width;
		mHeight = _height;

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

		// Enable 4x MSAA (anti-aliasing)
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		mRaw = SDL_CreateWindow("My Engine",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			mWidth, mHeight,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);


		mContext = SDL_GL_CreateContext(mRaw);

		if (!mContext)
		{
			std::cout << "Couldn't create SDL window." << std::endl;
			throw std::exception("Couldn't create SDL window.");
		}

		if (glewInit() != GLEW_OK)
		{
			std::cout << "Couldn't initialise glew." << std::endl;
			throw std::exception("Couldn't initialise glew.");
		}

		// 0 = no vsync, 1 = vsync, 2 = half of vsync
		// (using a value other than 0 on a monitor without gsync/freesync feels very slow)
		SDL_GL_SetSwapInterval(mVSync ? 1 : 0);

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glClearColor(mClearColour.r, mClearColour.g, mClearColour.b, 1.f);
	}

	Window::~Window()
	{
		SDL_GL_DeleteContext(mContext);
		SDL_DestroyWindow(mRaw);
		SDL_Quit();
	}

	void Window::Update()
	{
		SDL_GetWindowSize(mRaw, &mWidth, &mHeight);
		glViewport(0, 0, mWidth, mHeight);
	}

	void Window::ClearWindow()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Window::SetClearColour(glm::vec3 _colour)
	{
		mClearColour = _colour;
		glClearColor(mClearColour.r, mClearColour.g, mClearColour.b, 1.f);
	}

}