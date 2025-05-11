#pragma once

#include <SDL2/sdl.h>

#include <vector>
#include <array>
#include <algorithm>

namespace JamesEngine
{
    class Controller
    {
    public:
        Controller();
		~Controller() { CloseController(); }

        void Update();

        bool IsButton(int button) const;
        bool IsButtonDown(int button) const;
        bool IsButtonUp(int button) const;

        float GetAxis(int axis) const;

        bool IsConnected() const { return mController != nullptr; }

        void SetRumble(float lowFreqStrength, float highFreqStrength, float _duration);

    private:
        friend class Input;

        void OpenController();
        void CloseController();

        SDL_GameController* mController = nullptr;

        std::vector<int> mButtons;
        std::vector<int> mButtonsDown;
        std::vector<int> mButtonsUp;
        std::array<Sint16, SDL_CONTROLLER_AXIS_MAX> mAxisValues;
    };
}
