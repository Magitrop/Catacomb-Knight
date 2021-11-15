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

class IAnimatableObject
{
protected:
	AnimationFrame *curAnimationFrame = nullptr;
	int curAnimationFrameIndex = 0;
	double curAnimationTime = 0;
	SDL_Rect textureSrc;

	virtual void InitializeAnimations(void) = 0;
	//virtual void SetAnimation(AnimationFrame ***animationToPlay, bool isInterruptible = false, AnimationFrame ***previousAnimation = nullptr) = 0;
	virtual void PlayAnimations(void) = 0;
public:
	AnimationFrame **lastPlayedAnimation;

	void SetAnimationTime(double value)
	{
		curAnimationTime = value;
	}
	double GetAnimationTime()
	{
		return curAnimationTime;
	}

	void SetAnimationFrameIndex(int value)
	{
		curAnimationFrameIndex = value;
	}

	SDL_Rect* GetTextureSourceRect()
	{
		return &textureSrc;
	}

	/// <summary>
	/// coordinates for the animation render
	/// </summary>
	double animationCoordX, animationCoordY;

	void CalculateAnimationCoordinates(double visX, double visY)
	{
		if (curAnimationFrame == nullptr)
			return;
		animationCoordX = visX + Initializer.TEXTURE_RES * ((curAnimationFrame->resolutionX / Initializer.TEXTURE_RES) * curAnimationFrame->pivotX - 0.5);
		animationCoordY = visY + Initializer.TEXTURE_RES * ((curAnimationFrame->resolutionY / Initializer.TEXTURE_RES) * curAnimationFrame->pivotY - 0.5);
	}

	/// <summary>
	/// returns part of object texture (current animation frame) to render
	/// </summary>
	void PlayAnimation(int framesCount, Animation *framesSequence, Animation **lastPlayedAnimation, bool resetAnimationTimer)
	{
		if (framesSequence == nullptr)
			return;
		if ((resetAnimationTimer && *lastPlayedAnimation != framesSequence) ||
			curAnimationFrame == nullptr)
		{
			curAnimationFrameIndex /*= curAnimationTime*/ = 0;
			curAnimationFrame = framesSequence->frames[0];
		}
		*lastPlayedAnimation = framesSequence;
		textureSrc.x = curAnimationFrame->sourceRect->x;
		textureSrc.y = curAnimationFrame->sourceRect->y;
		textureSrc.w = curAnimationFrame->resolutionX;
		textureSrc.h = curAnimationFrame->resolutionY;

		curAnimationTime += Time::GetDeltaTime();
		if (curAnimationTime >= framesSequence->frameRate)
		{
			curAnimationTime -= framesSequence->frameRate;
			curAnimationFrameIndex++;
			if (curAnimationFrameIndex >= framesCount)
				curAnimationFrameIndex = 0;
			curAnimationFrame = framesSequence->frames[curAnimationFrameIndex];
		}
	}
	bool PlayAnimationOnce(int framesCount, Animation *framesSequence, Animation **lastPlayedAnimation, bool resetAnimationTimer)
	{
		if (framesSequence == nullptr)
			return false;
		if ((resetAnimationTimer && *lastPlayedAnimation != framesSequence) ||
			curAnimationFrame == nullptr)
		{
			curAnimationFrameIndex /*= curAnimationTime*/ = 0;
			curAnimationFrame = framesSequence->frames[0];
		}
		*lastPlayedAnimation = framesSequence;
		textureSrc.x = curAnimationFrame->sourceRect->x;
		textureSrc.y = curAnimationFrame->sourceRect->y;
		textureSrc.w = curAnimationFrame->resolutionX;
		textureSrc.h = curAnimationFrame->resolutionY;

		curAnimationTime += Time::GetDeltaTime();
		if (curAnimationTime >= framesSequence->frameRate)
		{
			curAnimationTime -= framesSequence->frameRate;
			curAnimationFrameIndex++;
			if (curAnimationFrameIndex >= framesCount)
			{
				curAnimationFrameIndex = 0;
				return true;
			}
			curAnimationFrame = framesSequence->frames[curAnimationFrameIndex];
		}
		return false;
	}

	/// <summary>
	/// returns part of object texture (current animation frame) to render
	/// </summary>
	[[deprecated]] void PlayAnimation(int framesCount, AnimationFrame ***framesSequence, int lineIndex, AnimationFrame ***lastPlayedAnimation = nullptr, double frameRate = 0.15)
	{
		if (framesSequence == nullptr)
			return;
		if (*lastPlayedAnimation != framesSequence[lineIndex])
		{
			curAnimationFrameIndex = curAnimationTime = 0;
			curAnimationFrame = framesSequence[lineIndex][0];
		}
		*lastPlayedAnimation = framesSequence[lineIndex];
		textureSrc.x = curAnimationFrame->sourceRect->x;
		textureSrc.y = curAnimationFrame->sourceRect->y;
		textureSrc.w = curAnimationFrame->resolutionX;
		textureSrc.h = curAnimationFrame->resolutionY;

		curAnimationTime += Time::GetDeltaTime();
		if (curAnimationTime >= frameRate)
		{
			curAnimationTime -= frameRate;
			curAnimationFrameIndex++;
			if (curAnimationFrameIndex >= framesCount)
				curAnimationFrameIndex = 0;
			curAnimationFrame = framesSequence[lineIndex][curAnimationFrameIndex];
		}
	}
	[[deprecated]] bool PlayAnimationOnce(int framesCount, AnimationFrame ***framesSequence, int lineIndex, AnimationFrame ***lastPlayedAnimation = nullptr, double frameRate = 0.15)
	{
		if (framesSequence == nullptr)
			return false;
		if (*lastPlayedAnimation != framesSequence[lineIndex])
		{
			curAnimationFrameIndex = curAnimationTime = 0;
			curAnimationFrame = framesSequence[lineIndex][0];
		}
		*lastPlayedAnimation = framesSequence[lineIndex];
		textureSrc.x = curAnimationFrame->sourceRect->x;
		textureSrc.y = curAnimationFrame->sourceRect->y;
		textureSrc.w = curAnimationFrame->resolutionX;
		textureSrc.h = curAnimationFrame->resolutionY;

		curAnimationTime += Time::GetDeltaTime();
		if (curAnimationTime >= frameRate)
		{
			curAnimationTime -= frameRate;
			curAnimationFrameIndex++;
			if (curAnimationFrameIndex >= framesCount)
			{
				curAnimationFrameIndex = 0;
				return true;
			}
			curAnimationFrame = framesSequence[lineIndex][curAnimationFrameIndex];
		}
		return false;
	}

	void PlayAnimation(int frameFrom, int frameTo, Animation* framesSequence, Animation** lastPlayedAnimation, bool resetAnimationTimer)
	{
		if (framesSequence == nullptr)
			return;
		if ((resetAnimationTimer && *lastPlayedAnimation != framesSequence) ||
			curAnimationFrame == nullptr)
		{
			curAnimationFrameIndex /*= curAnimationTime*/ = frameFrom;
			curAnimationFrame = framesSequence->frames[frameFrom];
		}
		*lastPlayedAnimation = framesSequence;
		textureSrc.x = curAnimationFrame->sourceRect->x;
		textureSrc.y = curAnimationFrame->sourceRect->y;
		textureSrc.w = curAnimationFrame->resolutionX;
		textureSrc.h = curAnimationFrame->resolutionY;

		curAnimationTime += Time::GetDeltaTime();
		if (curAnimationTime >= framesSequence->frameRate)
		{
			curAnimationTime -= framesSequence->frameRate;
			curAnimationFrameIndex++;
			if (curAnimationFrameIndex >= frameTo)
				curAnimationFrameIndex = frameFrom;
			curAnimationFrame = framesSequence->frames[curAnimationFrameIndex];
		}
	}
	bool PlayAnimationOnce(int frameFrom, int frameTo, Animation* framesSequence, Animation** lastPlayedAnimation, bool resetAnimationTimer)
	{
		if (framesSequence == nullptr)
			return false;
		if ((resetAnimationTimer && *lastPlayedAnimation != framesSequence) ||
			curAnimationFrame == nullptr)
		{
			curAnimationFrameIndex /*= curAnimationTime*/ = frameFrom;
			curAnimationFrame = framesSequence->frames[frameFrom];
		}
		*lastPlayedAnimation = framesSequence;
		textureSrc.x = curAnimationFrame->sourceRect->x;
		textureSrc.y = curAnimationFrame->sourceRect->y;
		textureSrc.w = curAnimationFrame->resolutionX;
		textureSrc.h = curAnimationFrame->resolutionY;

		curAnimationTime += Time::GetDeltaTime();
		if (curAnimationTime >= framesSequence->frameRate)
		{
			curAnimationTime -= framesSequence->frameRate;
			curAnimationFrameIndex++;
			if (curAnimationFrameIndex >= frameTo)
			{
				curAnimationFrameIndex = frameFrom;
				return true;
			}
			curAnimationFrame = framesSequence->frames[curAnimationFrameIndex];
		}
		return false;
	}
};