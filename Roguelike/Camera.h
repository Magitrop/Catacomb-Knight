#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "Tilemap.h"
#include "TilemapMath.h"
#include "GameInitializer.h"
#include "MapController.h"

class MapController;
class Camera final
{
private:
	bool cameraFinishedMovement = true;
public:
	// visual coordinates for the render
	double x;
	double y;

	// coordinates for the free camera
	double tileX;
	double tileY;

	// speed of approach to the target
	double cameraMovementSpeed;

	// is camera moving freely
	bool freeCamera;

	// target to follow
	TileObject *target;

	Camera(double startX = 0, double startY = 0, double moveSpeed = 350)
	{
		x = startX;
		y = startY;
		cameraMovementSpeed = moveSpeed;
		target = nullptr;
		cameraFinishedMovement = true;
	}

	bool CameraFollow(MapController *m);
	bool GetCameraMovementState(MapController *m);
	/// <summary>
	/// teleport to the target
	/// </summary>
	/// <param name="m"></param>
	void CameraTeleport(MapController *m);
	/// <summary>
	/// teleport to the position in coordinates
	/// </summary>
	/// <param name="m"></param>
	void CameraTeleport(MapController *m, double _x, double _y);
};