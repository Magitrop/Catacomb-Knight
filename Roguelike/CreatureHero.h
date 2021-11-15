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

class MapController;
class Hero : public Creature
{
private:
	double curHealthInHealthbar;
	double curEnergyInEnergybar;
protected:
	Animation** idleAnim;
	Animation** movementAnim;
	Animation** attackAnim;
	Animation** deathAnim;
	Animation** deadAnim;
	Animation* _lastPlayedAnimation;

	virtual void InitializeAnimations(void) override
	{
		idleAnim = new Animation * [4]
		{
			// facing left
			new Animation(0.2, 4,
			{
				new AnimationFrame(16, 16, 0 * 16, 0 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 1 * 16, 0 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 2 * 16, 0 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 1 * 16, 0 * 16, 0.5, 0.5),
			}),
			// facing right
			new Animation(0.2, 4,
			{
				new AnimationFrame(16, 16, 0 * 16, 1 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 1 * 16, 1 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 2 * 16, 1 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 1 * 16, 1 * 16, 0.5, 0.5),
			}),
			// facing down
			new Animation(0.2, 4,
			{
				new AnimationFrame(16, 16, 0 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 1 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 2 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 1 * 16, 2 * 16, 0.5, 0.5),
			}),
			// facing up
			new Animation(0.2, 4,
			{
				new AnimationFrame(16, 16, 0 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 1 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 2 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 1 * 16, 3 * 16, 0.5, 0.5),
			}),
		};
		movementAnim = new Animation * [4]
		{
			// facing left
			new Animation(0.15, 4,
			{
				new AnimationFrame(16, 16, 13 * 16, 0 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 0 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 15 * 16, 0 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 0 * 16, 0.5, 0.5),
			}),
			// facing right
			new Animation(0.15, 4,
			{
				new AnimationFrame(16, 16, 13 * 16, 1 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 1 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 15 * 16, 1 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 1 * 16, 0.5, 0.5),
			}),
			// facing down
			new Animation(0.15, 4,
			{
				new AnimationFrame(16, 16, 13 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 15 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 2 * 16, 0.5, 0.5),
			}),
			// facing up
			new Animation(0.15, 4,
			{
				new AnimationFrame(16, 16, 13 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 15 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 3 * 16, 0.5, 0.5),
			}),
		};
		deathAnim = new Animation * [4]
		{
			// facing left
			new Animation(0.2, 4,
			{
				new AnimationFrame(16, 16, 12 * 16, 0, 0.5, 0.5),
				new AnimationFrame(16, 16, 13 * 16, 0, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 0, 0.5, 0.5),
				new AnimationFrame(16, 16, 15 * 16, 0, 0.5, 0.5)
			}),
			// facing right
			new Animation(0.2, 4,
			{
				new AnimationFrame(16, 16, 12 * 16, 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 13 * 16, 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 15 * 16, 16, 0.5, 0.5)
			}),
			// facing down
			new Animation(0.2, 4,
			{
				new AnimationFrame(16, 16, 12 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 13 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 2 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 15 * 16, 2 * 16, 0.5, 0.5)
			}),
			// facing up
			new Animation(0.2, 4,
			{
				new AnimationFrame(16, 16, 12 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 13 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 14 * 16, 3 * 16, 0.5, 0.5),
				new AnimationFrame(16, 16, 15 * 16, 3 * 16, 0.5, 0.5)
			}),
		};
		deadAnim = new Animation * [4]
		{
			// facing left
			new Animation(0.1, 1,
			{
				new AnimationFrame(16, 16, 15 * 16, 0, 0.5, 0.5),
			}),
			// facing right
			new Animation(0.1, 1,
			{
				new AnimationFrame(16, 16, 15 * 16, 16, 0.5, 0.5),
			}),
			// facing down
			new Animation(0.1, 1,
			{
				new AnimationFrame(16, 16, 15 * 16, 2 * 16, 0.5, 0.5),
			}),
			// facing up
			new Animation(0.1, 1,
			{
				new AnimationFrame(16, 16, 15 * 16, 3 * 16, 0.5, 0.5),
			}),
		};
		attackAnim = new Animation * [4]
		{
			// facing left
			new Animation(0.1, 6,
			{
				new AnimationFrame(32, 16, 3 * 16, 0, 0.75, 0.5),
				new AnimationFrame(32, 16, 5 * 16, 0, 0.75, 0.5),
				new AnimationFrame(32, 16, 7 * 16, 0, 0.75, 0.5),
				new AnimationFrame(32, 16, 9 * 16, 0, 0.75, 0.5),
				new AnimationFrame(32, 16, 11 * 16, 0, 0.75, 0.5),
				new AnimationFrame(32, 16, 3 * 16, 0, 0.75, 0.5)
			}),
			// facing right
			new Animation(0.1, 6,
			{
				new AnimationFrame(32, 16, 3 * 16, 1 * 16, 0.25, 0.5),
				new AnimationFrame(32, 16, 5 * 16, 1 * 16, 0.25, 0.5),
				new AnimationFrame(32, 16, 7 * 16, 1 * 16, 0.25, 0.5),
				new AnimationFrame(32, 16, 9 * 16, 1 * 16, 0.25, 0.5),
				new AnimationFrame(32, 16, 11 * 16, 1 * 16, 0.25, 0.5),
				new AnimationFrame(32, 16, 3 * 16, 1 * 16, 0.25, 0.5)
			}),
			// facing down
			new Animation(0.1, 6,
			{
				new AnimationFrame(16, 32, 3 * 16, 2 * 16, 0.5, 0.25),
				new AnimationFrame(16, 32, 4 * 16, 2 * 16, 0.5, 0.25),
				new AnimationFrame(16, 32, 5 * 16, 2 * 16, 0.5, 0.25),
				new AnimationFrame(16, 32, 6 * 16, 2 * 16, 0.5, 0.25),
				new AnimationFrame(16, 32, 7 * 16, 2 * 16, 0.5, 0.25),
				new AnimationFrame(16, 16, 0 * 16, 2 * 16, 0.5, 0.5)
			}),
			// facing up
			new Animation(0.1, 6,
			{
				new AnimationFrame(16, 32, 8 * 16, 2 * 16, 0.5, 0.75),
				new AnimationFrame(16, 32, 9 * 16, 2 * 16, 0.5, 0.75),
				new AnimationFrame(16, 32, 10 * 16, 2 * 16, 0.5, 0.75),
				new AnimationFrame(16, 32, 11 * 16, 2 * 16, 0.5, 0.75),
				new AnimationFrame(16, 32, 12 * 16, 2 * 16, 0.5, 0.75),
				new AnimationFrame(16, 16, 0 * 16, 3 * 16, 0.5, 0.5)
			}),
		};
	}
	virtual void PlayAnimations(void) override
	{
		if (isAlive)
		{
			if (isAttacking)
				isAttacking = !PlayAnimationOnce(6, attackAnim[(int)facingDirection], &_lastPlayedAnimation, true);
			else if (GetMovementState())
				PlayAnimation(4, idleAnim[(int)facingDirection], &_lastPlayedAnimation, false);
			else PlayAnimation(4, movementAnim[(int)facingDirection], &_lastPlayedAnimation, true);
		}
		else
		{
			if (!playedDeathAnimation)
				playedDeathAnimation = PlayAnimationOnce(4, deathAnim[(int)facingDirection], &_lastPlayedAnimation, true);
			else PlayAnimation(1, deadAnim[(int)facingDirection], &_lastPlayedAnimation, false);
		}
	}

	virtual bool UpdateHealthbar(double* healthToShow, double updateSpeed = 125) override
	{
		double speedDelta = updateSpeed * Time::GetDeltaTime();
		Controller.playerHealthbarFullPosition->w =
			Controller.playerHealthbarSrc->w = (int)((min(*healthToShow, maxHealth) / (double)maxHealth) * 104);
		*healthToShow = MoveTowards(*healthToShow, curHealth, speedDelta);
		if (fabs(*healthToShow - curHealth) <= speedDelta)
			return true;
		return false;
	}
	bool UpdateEnergybar(double* energyToShow, double updateSpeed = 90)
	{
		double speedDelta = updateSpeed * Time::GetDeltaTime();
		Controller.playerEnergybarFullPosition->w =
			Controller.playerEnergybarSrc->w = (int)((min(*energyToShow, maxActionsCount) / maxActionsCount) * 104);
		*energyToShow = MoveTowards(*energyToShow, actionsLeft, speedDelta);
		if (fabs(*energyToShow - actionsLeft) <= speedDelta)
			return true;
		return false;
	}
	virtual bool GetDamage(int amount, TileObject* sender = nullptr) override
	{
		int reduction = 0;
		for (int i = 0; i < Inventory.slots.size(); i++)
			if (Inventory.slots[i]->GetItem() != nullptr)
				Inventory.slots[i]->GetItem()->OnGettingDamageWith(this, amount, sender, &reduction);
		if (!Controller.godMode)
			curHealth = (int)MoveTowards((int)curHealth, 0, max((int)amount - reduction, 1));
		return OnDamage(sender);
	}
public:
	virtual ~Hero() override
	{
		delete idleAnim;
		delete movementAnim;
		delete attackAnim;
		delete deathAnim;
		delete deadAnim;
	}

	virtual void OnTurnStart(void) override
	{
		map->camera.target = this;
		actionsLeft = maxActionsCount;
		//Controller.SetPause(0.25);
	}
	virtual void OnTurnEnd(void) override
	{

	}
	// return true if killed by damage
	virtual bool OnDamage(TileObject* sender = nullptr) override
	{
		if (curHealth <= 0)
		{
			isAlive = false;
			map->tilemap->GetTile(x, y)->SetCorpse(this);
			map->tilemap->GetTile(x, y)->SetTileObject(nullptr);
			//Controller.gameInPause = true;
			Controller.SetPause(1.5);
			if (Controller.splashScreen != nullptr)
				delete Controller.splashScreen;
			Controller.splashScreen = new GameController::SplashScreen(255, 1, 1, 3, 2.25, 1, Controller.gameOverSplashText);
			Controller.splashScreen->onFullFadeIn =
				[&]()
			{
				Sounds.PlaySoundOnce("sounds/death.wav");
				Controller.gameIsRunning = false;
			};
		}
		return !isAlive;
		//cout << "taken damage\n";
	}
	virtual void OnHealing(TileObject* sender = nullptr) override
	{
	}
	virtual void OnFullHealing(TileObject* sender = nullptr) override
	{
	}
	virtual void OnTargetChange() override
	{
	}
	virtual void Initialize(int withID) override
	{
		viewDistance = 4;
		sightDistance = 5;
		visualMovementSpeed = 30;
		curAnimationTime = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (0.5)));
		Creature::Initialize(withID);

		maxActionsCount = 3;
		maxHealth = curHealth = 100;
		curHealthInHealthbar = curHealth;
		curEnergyInEnergybar = actionsLeft;
		damage = 20;
		canInteractWith = false;

		texturePosition->w =
			(curAnimationFrame == nullptr ? Initializer.TEXTURE_RES : curAnimationFrame->resolutionX);
		texturePosition->h =
			(curAnimationFrame == nullptr ? Initializer.TEXTURE_RES : curAnimationFrame->resolutionY);
		auto surf = IMG_Load("textures/hero.png");
		objectTexture = SDL_CreateTextureFromSurface(Initializer.renderer, surf);
		SDL_FreeSurface(surf);
	}
	virtual void OnAttack(TileObject* target = nullptr) override
	{
		Creature::OnAttack();
	}
	virtual void OnInteractWith(TileObject* sender = nullptr) override {}
	virtual void Update(void) override
	{
		Creature::Update();
		PlayAnimations();
		UpdateHealthbar(&curHealthInHealthbar, maxHealth * 0.75);
		UpdateEnergybar(&curEnergyInEnergybar, maxActionsCount * 0.75);
	}
	virtual void DrawTexture(void) override
	{
		Creature::DrawTexture();
		/*if (facingDirection == 1)
		{
			animationCoordX = visualX + Initializer.TEXTURE_RES * ((24 / Initializer.TEXTURE_RES) * curAnimationFrame->pivotX - 0.5);
			animationCoordY = visualY + Initializer.TEXTURE_RES * ((16 / Initializer.TEXTURE_RES) * curAnimationFrame->pivotY - 0.5);
			texturePosition->x = (int)(map->camera.x - animationCoordX);
			texturePosition->y = (int)(map->camera.y - animationCoordY - Initializer.TEXTURE_RES / 4);
			texturePosition->w = 24;
			texturePosition->h = 16;
			if (map->tilemap->GetTile(x, y)->isExplored &&
				map->tilemap->GetTile(x, y)->isVisible)
				SDL_RenderCopy(Initializer.renderer, objectTexture, &textureSrc, texturePosition);
			Creature::DrawTexture();
		}
		else
		{
			animationCoordX = visualX + Initializer.TEXTURE_RES * ((24 / Initializer.TEXTURE_RES) * curAnimationFrame->pivotX - 0.5);
			animationCoordY = visualY + Initializer.TEXTURE_RES * ((16 / Initializer.TEXTURE_RES) * curAnimationFrame->pivotY - 0.5);
			texturePosition->x = (int)(map->camera.x - animationCoordX);
			texturePosition->y = (int)(map->camera.y - animationCoordY - Initializer.TEXTURE_RES / 4);
			texturePosition->w = 24;
			texturePosition->h = 16;
			if (map->tilemap->GetTile(x, y)->isExplored &&
				map->tilemap->GetTile(x, y)->isVisible)
				SDL_RenderCopy(Initializer.renderer, objectTexture, &textureSrc, texturePosition);
			Creature::DrawTexture();
		}*/
	}
};