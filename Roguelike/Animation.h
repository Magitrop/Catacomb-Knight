#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "TilemapMath.h"
#include "Time.h"
#include "GameInitializer.h"
#include "AnimationFrame.h"
#include "IAnimatableObject.h"
using namespace std;

class Creature;
class Animation final
{
private:
	int framesCount;
public:
	AnimationFrame** frames;
	const double frameRate;

	template <class T>
	Animation(double _frameRate, int _framesCount, initializer_list<T> args) : frameRate(_frameRate)
	{
		framesCount = _framesCount;
		frames = new AnimationFrame* [_framesCount];
		int i = 0;
		for (auto arg : args)
		{
			frames[i] = arg;
			i++;
		}
	}
	~Animation()
	{
		if (frames != nullptr)
		{
			for (int i = 0; i < framesCount; i++)
				if (frames[i] != nullptr)
					delete frames[i];
			delete frames;
		}
	}
};