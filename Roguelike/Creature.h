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

enum FacingDirection
{
	Left,
	Right,
	Down,
	Up,
	None
};

class MapController;
class Creature : public TileObject
{
protected:
	bool finishedMovement = true;
	bool isAttacking = false;
	bool playedDeathAnimation;
	Creature *target;

	virtual bool GetDamage(int amount, TileObject *sender = nullptr)
	{
		curHealth = (int)MoveTowards((int)curHealth, 0, (int)amount);
		return OnDamage(sender);
	}
	virtual void GetHealing(int amount, TileObject *sender = nullptr)
	{
		bool fullheal;
		curHealth = (int)MoveTowards((int)curHealth, (int)maxHealth, (int)amount, &fullheal);
		OnHealing(sender);
		if (fullheal) OnFullHealing(sender);
	}
	virtual bool UpdateHealthbar(double *healthToShow, double updateSpeed = 5) = 0;
public:
	int previousLayer;
	/// <summary>
	/// can player control this creature?
	/// </summary>
	bool isUnderPlayerControl;
	// index of faction that creature belongs to
	// 0 is player's faction
	int factionID;

	bool endedTurn = true;

	int maxHealth;
	int curHealth;
	int maxActionsCount;
	int damage;
	int dropShardsCount;
	bool isAlive;

	/// <summary>
	/// distance at which creature can spot the player
	/// </summary>
	int viewDistance;
	/// <summary>
	/// distance at which player can see anything
	/// </summary>
	int sightDistance;

	FacingDirection facingDirection = (FacingDirection)(rand() % 4);

	bool showHealthbar = true;
	SDL_Rect *healthbarPosition = new SDL_Rect();
	SDL_Rect *healthbarEmptyPosition = new SDL_Rect();
	SDL_Rect *healthbarSrc = new SDL_Rect();
	SDL_Rect *healthbarEmptySrc = new SDL_Rect();
	SDL_Rect *texturePosition = new SDL_Rect();
	SDL_Texture *healthbarTexture;

	virtual ~Creature() override
	{
		delete healthbarPosition,
			healthbarEmptyPosition,
			healthbarSrc,
			healthbarEmptySrc;
		SDL_DestroyTexture(healthbarTexture);
	}

	static bool DealDamage(int amount, Creature* receiver, TileObject* sender)
	{
		if (receiver == nullptr)
			return false;
		if (sender != nullptr)
			sender->OnAttack();
		return receiver->GetDamage(amount, sender);
	}
	static void Heal(int amount, Creature *receiver, TileObject *sender)
	{
		if (receiver == nullptr)
			return;
		receiver->GetHealing(amount, sender);
	}

	virtual Creature* GetTarget()
	{
		return target;
	}
	void SetTarget(Creature *_target)
	{
		OnTargetChange();
		target = _target;
	}

	/// <summary>
	/// check if this creature finished its movement
	/// </summary>
	bool GetMovementState()
	{
		return finishedMovement && !isAttacking;
	}
	void SetFacingDirection(Tile *destination)
	{
		if (x < destination->x) facingDirection = FacingDirection::Right;
		else if (x > destination->x) facingDirection = FacingDirection::Left;
		else if (y < destination->y) facingDirection = FacingDirection::Down;
		else if (y > destination->y) facingDirection = FacingDirection::Up;
		else facingDirection = FacingDirection::None;
	}
	FacingDirection GetRequiredFacingDirection(Tile *destination)
	{
		if (x < destination->x) return FacingDirection::Right;
		else if (x > destination->x) return FacingDirection::Left;
		else if (y < destination->y) return FacingDirection::Down;
		else if (y > destination->y) return FacingDirection::Up;
		else return FacingDirection::None;
	}
	virtual bool MoveTo(Tile *destination)
	{
		SetFacingDirection(destination);
		if (destination == nullptr ||
			destination->tileType == 1 ||
			destination->GetTileObject() != nullptr ||
			!map->tilemap->IsNeighbor(map->tilemap->GetTile(x, y), destination))
			return false;
		map->tilemap->GetTile(x, y)->SetTileObject(nullptr);
		x = destination->x;
		previousLayer = y;
		y = destination->y;
		map->tilemap->GetTile(x, y)->SetTileObject(this);
		finishedMovement = false;
		return true;
	}

	/// <summary>
	/// returns true if turn was started successfully
	/// </summary>
	bool StartTurn(void)
	{
		//if (endedTurn)
		{
			OnTurnStart();
			endedTurn = false;
			//return true;
		}
		return true;
		//else return false;
	}
	/// <summary>
	/// returns true if turn was ended successfully
	/// </summary>
	bool EndTurn(void)
	{
		if (GetMovementState() && map->camera.GetCameraMovementState(map) && !endedTurn)
		{
			OnTurnEnd();
			endedTurn = true;
			return true;
		}
		else return false;
	}

	virtual void OnTurnStart(void) = 0;
	virtual void OnTurnEnd(void) = 0;
	virtual bool OnDamage(TileObject *sender = nullptr) = 0;
	virtual void OnHealing(TileObject *sender = nullptr) = 0;
	virtual void OnFullHealing(TileObject *sender = nullptr) = 0;
	virtual void OnTargetChange(void) = 0;
	virtual void OnAttack(TileObject *target = nullptr) override
	{
		isAttacking = true;
	}

	virtual void Initialize(int withID) override
	{
		UUID = withID;
		InitializeAnimations();
		playedDeathAnimation = false;

		map->tilemap->GetTile(x, y)->SetTileObject(this);
	}
	virtual void Update(void) override
	{
		// do not calculate movement if it is not visible for player
		finishedMovement = CalculateVisualCoordinates(
			!map->tilemap->GetTile(x, y)->isExplored || 
			!map->tilemap->GetTile(x, y)->isVisible);
		CalculateAnimationCoordinates(visualX, visualY);
		texturePosition->x = (int)(map->camera.x - animationCoordX);
		texturePosition->y = (int)(map->camera.y - animationCoordY - Initializer.TEXTURE_RES / 4);
		texturePosition->w = curAnimationFrame == nullptr ? Initializer.TEXTURE_RES : curAnimationFrame->resolutionX;
		texturePosition->h = curAnimationFrame == nullptr ? Initializer.TEXTURE_RES : curAnimationFrame->resolutionY;
	}

	virtual void DrawTexture(void) override
	{
		if (map->tilemap->GetTile(x, y)->isExplored && 
			map->tilemap->GetTile(x, y)->isVisible)
			SDL_RenderCopy(Initializer.renderer, objectTexture, &textureSrc, texturePosition);
	}
	virtual void DrawHealthbar(void)
	{
		if (healthbarTexture == nullptr || 
			!(map->tilemap->GetTile(x, y)->isExplored &&
			map->tilemap->GetTile(x, y)->isVisible))
			return;
		healthbarPosition->x = healthbarEmptyPosition->x = (int)(map->camera.x - visualX) + (Initializer.TEXTURE_RES * 1 / 8);
		healthbarPosition->y = healthbarEmptyPosition->y = (int)(map->camera.y - visualY - Initializer.TEXTURE_RES / 3);
		SDL_RenderCopy(Initializer.renderer, healthbarTexture, healthbarEmptySrc, healthbarEmptyPosition);
		SDL_RenderCopy(Initializer.renderer, healthbarTexture, healthbarSrc, healthbarPosition);
	}
};
