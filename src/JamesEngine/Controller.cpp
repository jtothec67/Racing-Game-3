#include "Controller.h"

#include <iostream>

namespace JamesEngine
{
    Controller::Controller()
    {
		OpenController();
        mAxisValues.fill(0);
    }

    void Controller::Update()
    {
        mButtonsDown.clear();
        mButtonsUp.clear();
    }

    bool Controller::IsButton(int button) const
    {
        return std::find(mButtons.begin(), mButtons.end(), button) != mButtons.end();
    }

    bool Controller::IsButtonDown(int button) const
    {
        return std::find(mButtonsDown.begin(), mButtonsDown.end(), button) != mButtonsDown.end();
    }

    bool Controller::IsButtonUp(int button) const
    {
        return std::find(mButtonsUp.begin(), mButtonsUp.end(), button) != mButtonsUp.end();
    }

    float Controller::GetAxis(int axis) const
    {
        Sint16 raw = mAxisValues[axis];
        if (axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
            axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
        {
            return raw / 32767.0f;
        }
        return (raw < 0)
            ? (raw / 32768.0f)
            : (raw / 32767.0f);
    }

    void Controller::OpenController()
    {
        int numJoysticks = SDL_NumJoysticks();
        for (int i = 0; i < numJoysticks; ++i)
        {
            if (SDL_IsGameController(i))
            {
                mController = SDL_GameControllerOpen(i);
                if (mController)
                {
                    std::cout << "Controller connected: " << SDL_GameControllerName(mController) << std::endl;
                    break;
                }
            }
        }
    }

    void Controller::CloseController()
    {
        if (mController)
        {
            std::cout << "Controller disconnected" << std::endl;
            SDL_GameControllerRumble(mController, 0, 0, 0);
            SDL_GameControllerClose(mController);
            mController = nullptr;
            mButtons.clear();
            mButtonsDown.clear();
            mButtonsUp.clear();
            mAxisValues.fill(0);
        }
    }

	void Controller::SetRumble(float _lowFreqStrength, float _highFreqStrength, float _deltaTime)
	{
		if (mController)
		{
            Uint32 durationMs = static_cast<Uint32>(_deltaTime * 1000.0f);

			SDL_GameControllerRumble(mController, static_cast<Uint16>(_lowFreqStrength * 65535), static_cast<Uint16>(_highFreqStrength * 65535), durationMs);
		}
	}

}
