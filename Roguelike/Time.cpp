#include <SDL.h>
#include "Time.h"

Uint32 Time::timeSinceStart = 0;
double Time::timeSinceStartInSeconds = 0;
double Time::deltaTime = 0;
double Time::timeScale = 1;
Uint64 Time::NOW = 0;
Uint64 Time::LAST = 0;

void Time::Calculate(void)
{
    LAST = NOW;
    SetNow(SDL_GetPerformanceCounter());
    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency()) * timeScale;
    timeSinceStartInSeconds += deltaTime;
    timeSinceStart = SDL_GetTicks();
}

void Time::SetTimeScale(double _timeScale)
{
    timeScale = _timeScale < 0 ? 0 : _timeScale;
}

void Time::SetNow(Uint64 _NOW)
{
    NOW = _NOW;
}

double Time::GetTimeScale(void)
{
    return timeScale;
}

Uint32 Time::GetTimeSinceStart(void)
{
    return timeSinceStart;
}

double Time::GetTimeSinceStartInSeconds(void)
{
    return timeSinceStartInSeconds;
}

double Time::GetDeltaTime(void)
{
    return deltaTime;
}