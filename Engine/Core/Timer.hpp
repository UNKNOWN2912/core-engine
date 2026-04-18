#pragma once
#include "Core/Macro.hpp"
#include <chrono>

void StartGlobalTimer();
void StopGlobalTimer();
float GetGlobalTimeElapsed();


class Timer
{
    public: 
        void Start();
        void Stop();

        float GetElapsedTime();
        float GetDuration();
    private:
        std::chrono::time_point<std::chrono::system_clock> mStart;
        std::chrono::time_point<std::chrono::system_clock> mEnd;
        std::chrono::duration<float> mDuration;
};

class ScopedTimer
{
    public:
        ScopedTimer(std::string_view label);
        ~ScopedTimer();
    private:
        std::string mLabel;
        Timer mTimer;
};