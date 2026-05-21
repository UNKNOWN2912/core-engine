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

        Timer(){}
        Timer(bool start)
        {
            if(start)
            {
                Start();
            }
        }
    private:
        decltype(std::chrono::high_resolution_clock::now()) mStart;
        decltype(std::chrono::high_resolution_clock::now()) mEnd; 
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