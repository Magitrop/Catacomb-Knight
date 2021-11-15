#pragma once

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "TilemapMath.h"
#include "Time.h"
#include "GameInitializer.h"
#include "Animation.h"
#include "AnimationFrame.h"

class VisualEffect final
{
private:
	AnimationFrame* curAnimationFrame = nullptr;
	int curAnimationFrameIndex = 0;
	int prevAnimationFrameIndex = 0;
	double curAnimationTime = 0;
	Animation* framesSequence;
	int framesCount;
	double visualX, visualY;
public:
	string effectName;
	SDL_Rect textureSrc;
	SDL_Rect textureRect;
	double animationCoordX, animationCoordY;
	SDL_Texture* effectTexture;
	const int x, y;
	bool playBehindCreature;

	VisualEffect(Animation* animation, string name, string texturePath, int _framesCount, int _x, int _y, bool _playBehindCreature = false) : x(_x), y(_y)
	{
		framesSequence = animation;
		framesCount = _framesCount;

		effectName = name;
		playBehindCreature = _playBehindCreature;

		auto surf = IMG_Load(texturePath.c_str());
		effectTexture = SDL_CreateTextureFromSurface(Initializer.renderer, surf);
		SDL_FreeSurface(surf);

		curAnimationFrameIndex = prevAnimationFrameIndex /*= curAnimationTime*/ = 0;
		curAnimationFrame = framesSequence->frames[0];
	}
	~VisualEffect()
	{
		delete framesSequence;
		SDL_DestroyTexture(effectTexture);
	}

	AnimationFrame* GetCurrentFrame()
	{
		return curAnimationFrame;
	}
	int GetCurrentFrameIndex()
	{
		return curAnimationFrameIndex;
	}
	int GetPreviousFrameIndex()
	{
		return prevAnimationFrameIndex;
	}
	void CalculateAnimationCoordinates()
	{
		if (curAnimationFrame == nullptr)
			return;
		visualX = -x * Initializer.TEXTURE_RES;
		visualY = -y * Initializer.TEXTURE_RES;
		animationCoordX = visualX + Initializer.TEXTURE_RES * ((curAnimationFrame->resolutionX / Initializer.TEXTURE_RES) * curAnimationFrame->pivotX - 0.5);
		animationCoordY = visualY + Initializer.TEXTURE_RES * ((curAnimationFrame->resolutionY / Initializer.TEXTURE_RES) * curAnimationFrame->pivotY - 0.5);
	}
	void CalculateAnimationCoordinates(double visX, double visY)
	{
		if (curAnimationFrame == nullptr)
			return;
		visualX = visX;
		visualY = visY;
		animationCoordX = visualX + Initializer.TEXTURE_RES * ((curAnimationFrame->resolutionX / Initializer.TEXTURE_RES) * curAnimationFrame->pivotX - 0.5);
		animationCoordY = visualY + Initializer.TEXTURE_RES * ((curAnimationFrame->resolutionY / Initializer.TEXTURE_RES) * curAnimationFrame->pivotY - 0.5);
	}
	bool PlayAnimationOnce()
	{
		if (framesSequence == nullptr)
			return false;
		textureSrc.x = curAnimationFrame->sourceRect->x;
		textureSrc.y = curAnimationFrame->sourceRect->y;
		textureSrc.w = curAnimationFrame->resolutionX;
		textureSrc.h = curAnimationFrame->resolutionY;

		curAnimationTime += Time::GetDeltaTime();
		if (curAnimationTime >= framesSequence->frameRate)
		{
			curAnimationTime -= framesSequence->frameRate;
			prevAnimationFrameIndex = curAnimationFrameIndex++;
			if (curAnimationFrameIndex >= framesCount)
				return true;
			curAnimationFrame = framesSequence->frames[curAnimationFrameIndex];
			if (curAnimationFrame->action != nullptr)
				curAnimationFrame->action(curAnimationFrame->actionParameters);
		}
		return false;
	}
	void PlayAnimation()
	{
		if (framesSequence == nullptr)
			return;
		textureSrc.x = curAnimationFrame->sourceRect->x;
		textureSrc.y = curAnimationFrame->sourceRect->y;
		textureSrc.w = curAnimationFrame->resolutionX;
		textureSrc.h = curAnimationFrame->resolutionY;

		curAnimationTime += Time::GetDeltaTime();
		if (curAnimationTime >= framesSequence->frameRate)
		{
			curAnimationTime -= framesSequence->frameRate;
			prevAnimationFrameIndex = curAnimationFrameIndex++;
			if (curAnimationFrameIndex >= framesCount)
				curAnimationFrameIndex = 0;
			curAnimationFrame = framesSequence->frames[curAnimationFrameIndex];
			if (curAnimationFrame->action != nullptr)
				curAnimationFrame->action(curAnimationFrame->actionParameters);
		}
	}
};