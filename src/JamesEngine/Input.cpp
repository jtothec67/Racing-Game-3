#include "Input.h"

#include <iostream>

namespace JamesEngine
{

	Input::Input()
	{
		mKeyboard = std::make_shared<Keyboard>();
		mMouse = std::make_shared<Mouse>();
		mController = std::make_shared<Controller>();
	}

	void Input::Update()
	{
		mKeyboard->Update();
		mMouse->Update();
		mController->Update();
	}

	void Input::HandleInput(const SDL_Event& _event)
	{
		if (_event.type != SDL_MOUSEMOTION)
		{
			mMouse->mDelta = glm::ivec2(0);
		}

		switch (_event.type)
		{
		case SDL_MOUSEMOTION:
			mMouse->mXpos = _event.motion.x;
			mMouse->mYpos = _event.motion.y;
			break;

		case SDL_KEYDOWN:
			mKeyboard->mKeys.push_back(_event.key.keysym.sym);
			mKeyboard->mKeysDown.push_back(_event.key.keysym.sym);
			break;

		case SDL_KEYUP:
			mKeyboard->KeyBeenReleased(_event.key.keysym.sym);
			mKeyboard->mKeysUp.push_back(_event.key.keysym.sym);
			break;

		case SDL_MOUSEBUTTONDOWN:
			mMouse->mButtons.push_back(_event.button.button);
			mMouse->mButtonsDown.push_back(_event.button.button);
			break;

		case SDL_MOUSEBUTTONUP:
			mMouse->ButtonBeenReleased(_event.button.button);
			mMouse->mButtonsUp.push_back(_event.button.button);
			break;

		case SDL_CONTROLLERBUTTONDOWN:
			mController->mButtons.push_back(_event.cbutton.button);
			mController->mButtonsDown.push_back(_event.cbutton.button);
			break;

		case SDL_CONTROLLERBUTTONUP:
			mController->mButtons.erase(
				std::remove(mController->mButtons.begin(), mController->mButtons.end(), _event.cbutton.button),
				mController->mButtons.end()
			);
			mController->mButtonsUp.push_back(_event.cbutton.button);
			break;

		case SDL_CONTROLLERAXISMOTION:
			mController->mAxisValues[_event.caxis.axis] = _event.caxis.value;
			break;

		case SDL_CONTROLLERDEVICEADDED:
			if (!mController->mController) { // Only open if we don't have one
				mController->OpenController();
			}
			break;

		case SDL_CONTROLLERDEVICEREMOVED:
			if (mController->mController && _event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(mController->mController))) {
				mController->CloseController();
			}
			break;

		default:
			break;
		}
	}

}