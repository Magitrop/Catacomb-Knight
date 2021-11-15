#pragma once

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()

#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "TilemapMath.h"
#include "Tilemap.h"
#include "Time.h"
#include "Camera.h"
#include "MapController.h"
#include "GameController.h"
#include "AnimationFrame.h"
#include "Animation.h"

using namespace std;

class MapController;
class Boss : public Creature
{
protected:
	SDL_Rect healthbarForegroundSrc;
	SDL_Rect healthbarFullPosition;
	virtual bool UpdateHealthbar(double* healthToShow, double updateSpeed = 125) override
	{
		double speedDelta = updateSpeed * Time::GetDeltaTime();
		healthbarFullPosition.w =
			healthbarSrc->w = (int)((min(*healthToShow, maxHealth) / (double)maxHealth) * 208);
		*healthToShow = MoveTowards(*healthToShow, curHealth, speedDelta);
		if (fabs(*healthToShow - curHealth) <= speedDelta)
			return true;
		return false;
	}
public:
	virtual void Initialize(int withID) override
	{
		auto surf = IMG_Load("textures/boss_ui.png");

		healthbarSrc->x = 0;
		healthbarSrc->y = 32;
		healthbarSrc->w = 208;
		healthbarSrc->h = 32;

		healthbarEmptySrc->x = 0;
		healthbarEmptySrc->y = 64;
		healthbarEmptySrc->w = 208;
		healthbarEmptySrc->h = 32;

		healthbarPosition->w = healthbarFullPosition.w = (int)(curHealth / (double)maxHealth * 208);
		healthbarEmptyPosition->w = 176;
		healthbarPosition->h = healthbarFullPosition.h = healthbarEmptyPosition->h = 32;
		healthbarPosition->x = healthbarFullPosition.x = (Initializer.SCREEN_WIDTH / 4.5 - 208) / 2;
		healthbarPosition->y = healthbarFullPosition.y = Initializer.SCREEN_HEIGHT / 4.5 - 32;
		healthbarTexture = SDL_CreateTextureFromSurface(Initializer.renderer, surf);
		SDL_FreeSurface(surf);
		healthbarForegroundSrc = { 0, 0, 208, 32 };
	}

	virtual void DrawHealthbar(void) override
	{
		/*if (healthbarTexture == nullptr ||
			!(map->tilemap->GetTile(x, y)->isExplored &&
				map->tilemap->GetTile(x, y)->isVisible))
			return;*/

		SDL_RenderSetScale(Initializer.renderer, 4.5, 4.5);
		SDL_RenderCopy(Initializer.renderer, healthbarTexture, healthbarEmptySrc, healthbarPosition);
		SDL_RenderCopy(Initializer.renderer, healthbarTexture, healthbarSrc, &healthbarFullPosition);
		SDL_RenderCopy(Initializer.renderer, healthbarTexture, &healthbarForegroundSrc, healthbarPosition);
		SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
	}
};
