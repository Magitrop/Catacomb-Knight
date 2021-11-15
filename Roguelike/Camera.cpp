#define Initializer GameInitializer::Instance()

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "Tilemap.h"
#include "GameInitializer.h"
#include "MapController.h"

bool Camera::CameraFollow(MapController *map)
{
	if (freeCamera)
		return false;

	Tile* targetTile = map->tilemap->GetTile(target->x, target->y);
	if (target == nullptr || 
		targetTile == nullptr || 
		!targetTile->isVisible ||
		!targetTile->isExplored)
		return true;

	double speedDelta = cameraMovementSpeed * Time::GetDeltaTime();
	bool reachX = false, reachY = false;
	/*x = MoveTowards(x, target->visualX + Initializer.SCREEN_WIDTH / (2 * map->zoom) - 2 * map->zoom, speedDelta, &reachX);
	y = MoveTowards(y, target->visualY + Initializer.SCREEN_HEIGHT / (2 * map->zoom) - 2 * map->zoom, speedDelta, &reachY);*/
	double targetX = target->visualX + Initializer.SCREEN_WIDTH / (2 * map->zoom) - 2 * map->zoom;
	double targetY = target->visualY + Initializer.SCREEN_HEIGHT / (2 * map->zoom) - 2 * map->zoom;

	x = MoveTowards(x,
		clamp(targetX,
			-(map->mapSizeX * Initializer.TEXTURE_RES - Initializer.SCREEN_WIDTH / map->zoom),
			0.), speedDelta, &reachX);
	y = MoveTowards(y,
		clamp(targetY,
			-(map->mapSizeY * Initializer.TEXTURE_RES - Initializer.SCREEN_HEIGHT / map->zoom),
			0.), speedDelta, &reachY);
	cameraFinishedMovement = reachX && reachY;
	return cameraFinishedMovement;
}

bool Camera::GetCameraMovementState(MapController *map)
{
	if (freeCamera)
		return false;
	Tile* targetTile = map->tilemap->GetTile(target->x, target->y);
	if (target == nullptr ||
		targetTile == nullptr ||
		!targetTile->isVisible ||
		!targetTile->isExplored)
		return true;

	double speedDelta = cameraMovementSpeed * Time::GetDeltaTime();
	bool reachX = false, reachY = false;
	double targetX = target->visualX + Initializer.SCREEN_WIDTH / (2 * map->zoom) - 2 * map->zoom;
	double targetY = target->visualY + Initializer.SCREEN_HEIGHT / (2 * map->zoom) - 2 * map->zoom;

	reachX = CheckDistanceEquality(x, 
		clamp(targetX,
			-(map->mapSizeX * Initializer.TEXTURE_RES - Initializer.SCREEN_WIDTH / map->zoom),
			0.), speedDelta);
	reachY = CheckDistanceEquality(y, 
		clamp(targetY,
			-(map->mapSizeY * Initializer.TEXTURE_RES - Initializer.SCREEN_HEIGHT / map->zoom),
			0.), speedDelta);
	return reachX && reachY;
}

void Camera::CameraTeleport(MapController *map)
{
	if (target == nullptr)
		return;
	x = target->visualX + Initializer.SCREEN_WIDTH / (2 * map->zoom) - 2 * map->zoom;
	y = target->visualY + Initializer.SCREEN_HEIGHT / (2 * map->zoom) - 2 * map->zoom;
	cameraFinishedMovement = true;
}

void Camera::CameraTeleport(MapController *map, double _x, double _y)
{
	x = _x * Initializer.TEXTURE_RES + Initializer.SCREEN_WIDTH / (2 * map->zoom) - 2 * map->zoom;
	y = _y * Initializer.TEXTURE_RES + Initializer.SCREEN_HEIGHT / (2 * map->zoom) - 2 * map->zoom;
	cameraFinishedMovement = true;
}