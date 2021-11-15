#pragma once
#include <SDL.h>

class Time final
{
private:
	static double timeScale;
	static double deltaTime;
	static double timeSinceStartInSeconds;
	static Uint32 timeSinceStart;
	static Uint64 NOW, LAST;
public:
	static void Calculate(void);
	static void SetTimeScale(double _timeScale);
	static void SetNow(Uint64 _NOW);
	static double GetTimeScale(void);
	static Uint32 GetTimeSinceStart(void);
	static double GetTimeSinceStartInSeconds(void);
	static double GetDeltaTime(void);
};