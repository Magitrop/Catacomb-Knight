#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <functional>

using namespace std;

class AnimationFrame final
{
public:
	const int resolutionX;
	const int resolutionY;
	// [0, 1]
	const double pivotX;
	// [0, 1]
	const double pivotY;
	// do not change
	SDL_Rect *sourceRect;

	function<void(vector<void*>)> action;
	vector<void*> actionParameters;
	
	AnimationFrame(
		int resX, int resY, 
		int srcX, int srcY, 
		double pivX, double pivY) : 
		resolutionX(resX), resolutionY(resY), 
		pivotX(pivX), pivotY(pivY)
	{
		sourceRect = new SDL_Rect();
		sourceRect->x = srcX;
		sourceRect->y = srcY;
		sourceRect->w = resX;
		sourceRect->h = resY;
	}

	AnimationFrame(
		int resX, int resY,
		int srcX, int srcY,
		double pivX, double pivY,
		function<void(vector<void*>)> _action, vector<void*> params) :
		resolutionX(resX), resolutionY(resY),
		pivotX(pivX), pivotY(pivY)
	{
		sourceRect = new SDL_Rect();
		sourceRect->x = srcX;
		sourceRect->y = srcY;
		sourceRect->w = resX;
		sourceRect->h = resY;
		action = _action;
		actionParameters = params;
	}
	~AnimationFrame()
	{
		delete sourceRect;
	}
};