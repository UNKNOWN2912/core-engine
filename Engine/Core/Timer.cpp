#include "Timer.hpp"


void Timer::Start()
{
    mStart = std::chrono::high_resolution_clock::now();
}
void Timer::Stop()
{
    mEnd = std::chrono::high_resolution_clock::now();
}
float Timer::GetElapsedTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> dur;

    dur = currentTime - mStart;

    return dur.count();
    
}
float Timer::GetDuration()
{
    mDuration = mEnd - mStart;
    return mDuration.count();
}
ScopedTimer::ScopedTimer(std::string_view label) 
{ 
    mLabel = label; 
    mTimer.Start(); 
}

ScopedTimer::~ScopedTimer() 
{ 
    mTimer.Stop(); 
    LOG("Scoped Timer [{}]: {}", mLabel, mTimer.GetDuration()); 
}

static Timer globalTimer;

void StartGlobalTimer() 
{
    globalTimer.Start();    
}

void StopGlobalTimer() 
{
    globalTimer.Stop();
}

float GetGlobalTimeElapsed() 
{
    return globalTimer.GetElapsedTime();
}

