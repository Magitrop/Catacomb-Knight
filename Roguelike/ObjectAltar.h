#pragma once

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()

#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "TilemapMath.h"
#include "Tilemap.h"
#include "Time.h"
#include "Creature.h"
#include "Camera.h"
#include "MapController.h"
#include "GameController.h"
#include "GameInitializer.h"
#include "Animation.h"
#include "MapHandler.h"

class MapController;
class ObjectAltar : public TileObject
{
private:
	int curPathIndex = 0;
	List<Tile*>* curPath;
	Tile* tileToMove;

	Animation** idleAnim;
	Animation** filledAnim;
	Animation* _lastPlayedAnimation;

	double curHealthInHealthbar;

	int *shardCountPhases;
protected:
	virtual void InitializeAnimations(void) override
	{
		idleAnim = new Animation * [1]
		{
			new Animation(0.2, 5,
			{
				new AnimationFrame(16, 24, 0 * 16, 0 * 24, 0.5, 1),
				new AnimationFrame(16, 24, 1 * 16, 0 * 24, 0.5, 1),
				new AnimationFrame(16, 24, 2 * 16, 0 * 24, 0.5, 1),
				new AnimationFrame(16, 24, 3 * 16, 0 * 24, 0.5, 1),
				new AnimationFrame(16, 24, 4 * 16, 0 * 24, 0.5, 1),
			})
		};
		filledAnim = new Animation * [1]
		{
			new Animation(0.2, 5,
			{
				new AnimationFrame(16, 24, 0 * 16, 1 * 24, 0.5, 1),
				new AnimationFrame(16, 24, 1 * 16, 1 * 24, 0.5, 1),
				new AnimationFrame(16, 24, 2 * 16, 1 * 24, 0.5, 1),
				new AnimationFrame(16, 24, 3 * 16, 1 * 24, 0.5, 1),
				new AnimationFrame(16, 24, 4 * 16, 1 * 24, 0.5, 1),
			}),
		};
	}
	virtual void PlayAnimations(void) override
	{
		for (int i = 0; i < 5; i++)
			if (Controller.shardsCount < shardCountPhases[i])
			{
				PlayAnimation(i, i, idleAnim[0], &_lastPlayedAnimation, false);
				return;
			}
		PlayAnimation(5, filledAnim[0], &_lastPlayedAnimation, false);
	}
public:
	virtual ~ObjectAltar() override
	{
		delete[] idleAnim;
		delete filledAnim;
	}

	virtual void OnAttack(TileObject* target = nullptr) override
	{
		//Creature::OnAttack();
	}
	virtual void OnInteractWith(TileObject* sender = nullptr) override
	{
		if (Controller.shardsCount >= Controller.shardsRequired)
			Controller.NextDay();
	}
	virtual void Initialize(int withID) override
	{
		UUID = withID;
		InitializeAnimations();
		curAnimationTime = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (0.5)));

		auto surf = IMG_Load("textures/altar.png");
		objectTexture = SDL_CreateTextureFromSurface(Initializer.renderer, surf);
		SDL_FreeSurface(surf);

		shardCountPhases = new int[5];
		for (int i = 0; i < 5; i++)
			shardCountPhases[i] = Controller.shardsRequired * (i + 1) / 5;
		canInteractWith = false;
	}
	virtual void DrawTexture(void) override
	{
		if (curAnimationFrame == nullptr)
			return;
		if (map->tilemap->GetTile(x, y)->isExplored &&
			map->tilemap->GetTile(x, y)->isVisible)
			SDL_RenderCopy(Initializer.renderer, objectTexture, &textureSrc, texturePosition);
	}
	virtual void Update(void) override
	{
		if (map->tilemap->GetTile(x, y)->isExplored &&
			map->tilemap->GetTile(x, y)->isVisible)
			PlayAnimations();
		CalculateAnimationCoordinates(visualX, visualY);
		if (curAnimationFrame == nullptr)
			return;
		texturePosition->x = (int)(map->camera.x - animationCoordX);
		texturePosition->y = (int)(map->camera.y - animationCoordY - Initializer.TEXTURE_RES / 4);
		texturePosition->w = curAnimationFrame->resolutionX;
		texturePosition->h = curAnimationFrame->resolutionY;
	}
};