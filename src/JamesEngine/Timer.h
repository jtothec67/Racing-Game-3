#pragma once

#include <chrono>

class Timer
{
public:
    Timer() { }
    ~Timer() { }

    void Start(); // Set start time
    float Stop(); // Get time passed and stop the timer
    float GetElapsedSeconds(); // Get time passed without stopping the timer
    float GetElapsedMilliseconds(); // Get time passed in milliseconds without stopping the timer

    bool IsRunning() { return mRunning; }

private:
    std::chrono::steady_clock::time_point mStart;
    std::chrono::steady_clock::time_point mEnd;
    std::chrono::steady_clock::time_point mTempTimeStamp;

    bool mRunning = false;
};