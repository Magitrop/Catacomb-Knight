#pragma once

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <functional>
#include "TilemapMath.h"
#include "Time.h"
#include "GameInitializer.h"
#include "AnimationFrame.h"
#include "IAnimatableObject.h"
#include "VisualEffect.h"
using namespace std;

class MapController;
class Creature;
class Vector final
{
public:
	double x;
	double y;

	Vector(double _x, double _y) : x(_x), y(_y) {}
	Vector(int _x, int _y) : x(_x), y(_y) {}

	static double GetDistance(Vector& a, Vector& b)
	{
		return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	static double GetDistance(Vector a, Vector b)
	{
		return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}
};

class TileObject : public IAnimatableObject
{
private:
protected:
	/// <summary>
	/// UUID of a tile object
	/// </summary>
	int UUID;
public:
	/// <summary>
	/// exact coordinates on the map
	/// </summary>
	int x, y;

	/// <summary>
	/// visual coordinates for the movement
	/// </summary>
	double visualX, visualY;

	/// <summary>
	/// speed of approach of visual coordinates to real
	/// </summary>
	double visualMovementSpeed = 60;

	/// <summary>
	/// remaining count of actions that object can do
	/// </summary>
	int actionsLeft;

	/// <summary>
	/// reference to the map
	/// </summary>
	MapController *map;

	/// <summary>
	/// the texture of the object (including animation texture sets)
	/// </summary>
	SDL_Texture *objectTexture;

	/// <summary>
	///  array of currently playing visual effects
	/// </summary>
	vector<VisualEffect*> visualEffects;

	SDL_Rect* texturePosition = new SDL_Rect();
	bool canInteractWith;

	/// <summary>
	/// returns true when visual movement is finished
	/// </summary>
	bool CalculateVisualCoordinates(bool skipCalculation)
	{
		bool result = false;
		double speedDelta = visualMovementSpeed * Time::GetDeltaTime();

		// if this is the last method call (and visual movement will be finished)
		if (fabs(-x * Initializer.TEXTURE_RES - visualX) <= speedDelta &&
			fabs(-y * Initializer.TEXTURE_RES - visualY) <= speedDelta)
			result = true;

		if (skipCalculation)
		{
			visualX = -x * Initializer.TEXTURE_RES;
			visualY = -y * Initializer.TEXTURE_RES;
			result = true;
		}
		else
		{
			visualX = MoveTowards(visualX, -x * Initializer.TEXTURE_RES, speedDelta);
			visualY = MoveTowards(visualY, -y * Initializer.TEXTURE_RES, speedDelta);
		}

		return result;
	}

	virtual void Initialize(int withID) = 0;
	virtual void Update(void) = 0;
	virtual void OnInteractWith(TileObject* sender = nullptr) = 0;
	virtual ~TileObject(void) 
	{
		delete texturePosition;
		SDL_DestroyTexture(objectTexture);
	}
	virtual void DrawTexture(void) = 0;

	virtual void OnAttack(TileObject* target = nullptr) {}

	Vector ToVector()
	{
		Vector result(x, y);
		return result;
	}

	int GetID()
	{
		return UUID;
	}
};

class TileItem final
{
public:
	SDL_Point coords;
	SDL_Rect textureSrc;
	SDL_Rect textureRect;
	SDL_Texture* texture;
	string connectedItemIdName;

	function<void(TileObject*, string)> OnPickUp;

	TileItem(
		SDL_Point _coords, 
		SDL_Rect src, 
		SDL_Texture* _texture,
		string _connectedItemIdName,
		function<void(TileObject*, string)> func)
	{
		connectedItemIdName = _connectedItemIdName;
		coords = _coords;
		textureSrc = src;
		textureRect = { 0, 0, Initializer.TEXTURE_RES, Initializer.TEXTURE_RES };
		texture = _texture;
		OnPickUp = func;
	}

	template<class T, class = typename enable_if<is_base_of<MapController, T>::value || is_null_pointer<T>::value>::type>
	void Update(T* map)
	{
		textureRect.x = (int)(((MapController*)map)->camera.x + coords.x * Initializer.TEXTURE_RES);
		textureRect.y = (int)(((MapController*)map)->camera.y + coords.y * Initializer.TEXTURE_RES - Initializer.TEXTURE_RES / 4);
	}
	void Render(Uint8 alpha)
	{
		//cout << textureSrc.x << " " << textureSrc.y << " " << textureSrc.w << " " << textureSrc.h << " " << endl;
		//cout << textureRect.x << " " << textureRect.y << " " << endl;
		//cout << endl;
		SDL_SetTextureAlphaMod(texture, alpha);
		SDL_RenderCopy(Initializer.renderer, texture, &textureSrc, &textureRect);
		SDL_SetTextureAlphaMod(texture, 255);
	}
};

class Tile final
{
private:
	TileObject *tileObject = nullptr;
	TileObject *corpse = nullptr;
public:
	const int x;
	const int y;
	//SDL_Point tileTextureSrcCoords;
	TileItem *tileItem = nullptr;

	enum class GenerationTileType
	{
		Unset,
		// wall that will never change
		TrueWall,
		// common wall, could change
		Wall,
		// common floor, could change
		Floor,
		// floor that will never change
		TrueFloor
	} generationTileType;

	// -1 - unset
	//  0 - floor
	//  1 - wall
	int tileType = -1;
	int pathIndex = -1;
	int pathCost = 1;
	int zoneNumber = -1;

	// -1 - is not behind any door
	int doorIndex = -1;
	// -1 - unset
	// 0 - blue
	// 1 - green
	// 2 - red
	// 3 - purple
	// 4 - yellow
	int doorColor;
	bool canPlaceKey;
	bool canPlaceItem;
	string requiredItemIdNameToUnlock;
	
	bool isExplored;
	bool isVisible;
	bool canSpawnCreature;

	Tile(void) : x(0), y(0) {}
	Tile(int tileX, int tileY) : x(tileX), y(tileY)
	{
		tileObject = nullptr;
		tileType = pathIndex = zoneNumber = -1;
		generationTileType = GenerationTileType::Unset;
		pathCost = 1;
		isExplored = isVisible = canSpawnCreature = false;
	}
	~Tile()
	{
		delete tileObject;
		tileObject = nullptr;
	}

	void PlaceTileItem(TileItem* item)
	{
		if (tileItem == nullptr && tileType == 0)
			tileItem = item;
	}
	void PickUpTileItem(TileObject* receiver, string connectedItemIdName)
	{
		if (tileItem != nullptr && tileType == 0)
		{
			if (receiver != nullptr)
				tileItem->OnPickUp(receiver, connectedItemIdName);
			delete tileItem;
			tileItem = nullptr;
		}
	}

	TileObject* GetTileObject(void)
	{
		return tileObject;
	}
	void SetTileObject(TileObject *newObject)
	{
		tileObject = newObject;
	}
	TileObject* GetCorpse(void)
	{
		return corpse;
	}
	void SetCorpse(TileObject *newCorpse)
	{
		corpse = newCorpse;
	}
	Tile operator = (Tile tile)
	{
		int *p = (int *)&x;
		*p = tile.x;

		p = (int *)&y;
		*p = tile.y;

		tileObject = tile.tileObject;
		return tile;
	}
};

enum FacingDirection;
class Tilemap final
{
private:
	Tile **map;
public:
	const int mapSizeX;
	const int mapSizeY;

	Tilemap(int sizeX, int sizeY) : mapSizeX(sizeX), mapSizeY(sizeY)
	{
		map = (Tile**)calloc(mapSizeY, sizeof(Tile*));
		for (int i = 0; i < mapSizeX; i++)
			map[i] = (Tile*)calloc(mapSizeX, sizeof(Tile));
		for (int x = 0; x < mapSizeX; x++)
			for (int y = 0; y < mapSizeY; y++)
			{
				map[x][y] = Tile(x, y);
				map[x][y].tileType = -1;
				map[x][y].pathIndex = -1;
				map[x][y].pathCost = 1;
				map[x][y].zoneNumber = -1;
				map[x][y].doorColor = -1;
				map[x][y].doorIndex = -1;
				map[x][y].canSpawnCreature = true;
				map[x][y].canPlaceKey = true;
				map[x][y].canPlaceItem = true;
			}
	}

	~Tilemap()
	{
		for (int i = 0; i < mapSizeX; i++)
			delete[] map[i];
		delete[] map;
	}

	Tile* GetTile(int x, int y)
	{
		if (x >= 0 && y >= 0 && x < mapSizeX && y < mapSizeY)
			return &map[x][y];
		else return nullptr;
	}

	bool IsNeighbor(Tile* a, Tile* b)
	{
		return ((a->x + 1 == b->x && a->y == b->y) ||
			(a->x - 1 == b->x && a->y == b->y) ||
			(a->x == b->x && a->y + 1 == b->y) ||
			(a->x == b->x && a->y - 1 == b->y));
	}

	template<class T, class = typename enable_if<is_base_of<Creature, T>::value || is_null_pointer<T>::value>::type>
	List<Tile*>* FindPath(Tile *from, Tile *to, T *sender)
	{
		if ((from->x == to->x && from->y == to->y) ||
			to->tileType == 1 || to->doorColor != -1 ||
			from == nullptr || to == nullptr)
			return nullptr;

		List<Tile*>* path = new List<Tile*>();
		from->pathIndex = 0;

		List<Tile*> openedCells;
		List<Tile*> closedCells;
		openedCells.Add(from);

		int iterationsLeft;
		if (sender != nullptr)
			iterationsLeft = ((Creature*)sender)->viewDistance;
		else iterationsLeft = mapSizeX * mapSizeY;

		Tile* cur = from;
		Tile* neighborTile;
		int curMinPathIndex = -1;
		int lastMinPathIndex = -1;
		while (/*CheckNeighborsToPathIndex(cur) && */iterationsLeft > 0)
		{
			neighborTile = GetTile(cur->x, cur->y + 1);
			if (neighborTile == to ||
				(cur->y + 1 < mapSizeY &&
					neighborTile != nullptr &&
					neighborTile->tileType != 1 &&
					neighborTile->doorColor == -1 &&
					neighborTile->pathIndex == -1 &&
					neighborTile->GetTileObject() == nullptr &&
					openedCells.Contains(neighborTile) == false))
			{
				neighborTile->pathIndex = cur->pathIndex + neighborTile->pathCost;
				openedCells.Add(neighborTile);
			}

			neighborTile = GetTile(cur->x, cur->y - 1);
			if (neighborTile == to ||
				(cur->y - 1 >= 0 &&
					neighborTile != nullptr &&
					neighborTile->tileType != 1 &&
					neighborTile->doorColor == -1 &&
					neighborTile->pathIndex == -1 &&
					neighborTile->GetTileObject() == nullptr &&
					openedCells.Contains(neighborTile) == false))
			{
				neighborTile->pathIndex = cur->pathIndex + neighborTile->pathCost;
				openedCells.Add(neighborTile);
			}

			neighborTile = GetTile(cur->x + 1, cur->y);
			if (neighborTile == to ||
				(cur->x + 1 < mapSizeX &&
					neighborTile != nullptr &&
					neighborTile->tileType != 1 &&
					neighborTile->doorColor == -1 &&
					neighborTile->pathIndex == -1 &&
					neighborTile->GetTileObject() == nullptr &&
					openedCells.Contains(neighborTile) == false))
			{
				neighborTile->pathIndex = cur->pathIndex + neighborTile->pathCost;
				openedCells.Add(neighborTile);
			}

			neighborTile = GetTile(cur->x - 1, cur->y);
			if (neighborTile == to ||
				(cur->x - 1 >= 0 &&
					neighborTile != nullptr &&
					neighborTile->tileType != 1 &&
					neighborTile->doorColor == -1 &&
					neighborTile->pathIndex == -1 &&
					neighborTile->GetTileObject() == nullptr &&
					openedCells.Contains(neighborTile) == false))
			{
				neighborTile->pathIndex = cur->pathIndex + neighborTile->pathCost;
				openedCells.Add(neighborTile);
			}

			closedCells.Add(cur);
			if (openedCells.Contains(to))
				break;

			curMinPathIndex = -1;
			for (int i = 0; i < openedCells.Count(); i++)
			{
				if (closedCells.Contains(openedCells[i]))
					continue;
				if (curMinPathIndex == -1 || curMinPathIndex > openedCells[i]->pathIndex)
				{
					cur = openedCells[i];
					curMinPathIndex = cur->pathIndex;
				}
			}
			if (curMinPathIndex == -1)
				break;
			if (curMinPathIndex > lastMinPathIndex)
				iterationsLeft--;
			lastMinPathIndex = curMinPathIndex;
		}

		if (openedCells.Contains(to) == false)
		{
			delete path;
			path = nullptr;
			for (int i = 0; i < openedCells.Count(); i++)
				(*openedCells[i]).pathIndex = -1;
			return nullptr;
		}

		Tile* toGo = nullptr;
		cur = to;
		int minNumber = cur->pathIndex;

		path->Add(to);
		while (cur != from)
		{
			neighborTile = GetTile(cur->x, cur->y + 1);
			if (neighborTile == from ||
				(cur->y + 1 < mapSizeY &&
					neighborTile != nullptr &&
					neighborTile->pathIndex != -1 &&
					neighborTile->pathIndex < minNumber))
			{
				toGo = neighborTile;
				minNumber = toGo->pathIndex;
			}

			neighborTile = GetTile(cur->x, cur->y - 1);
			if (neighborTile == from ||
				(cur->y - 1 >= 0 &&
					neighborTile != nullptr &&
					neighborTile->pathIndex != -1 &&
					neighborTile->pathIndex < minNumber))
			{
				toGo = neighborTile;
				minNumber = toGo->pathIndex;
			}

			neighborTile = GetTile(cur->x + 1, cur->y);
			if (neighborTile == from ||
				(cur->x + 1 < mapSizeX &&
					neighborTile != nullptr &&
					neighborTile->pathIndex != -1 &&
					neighborTile->pathIndex < minNumber))
			{
				toGo = neighborTile;
				minNumber = toGo->pathIndex;
			}

			neighborTile = GetTile(cur->x - 1, cur->y);
			if (neighborTile == from ||
				(cur->x - 1 >= 0 &&
					neighborTile != nullptr &&
					neighborTile->pathIndex != -1 &&
					neighborTile->pathIndex < minNumber))
			{
				toGo = neighborTile;
				minNumber = toGo->pathIndex;
			}

			if (toGo != from)
				path->Add(toGo);
			cur = toGo;
			toGo = nullptr;
		}

		for (int i = 0; i < openedCells.Count(); i++)
			(*openedCells[i]).pathIndex = -1;
		openedCells.RemoveList();
		return path;
	}
	template<class T, class = typename enable_if<is_base_of<Creature, T>::value || is_null_pointer<T>::value>::type>
	List<Tile*>* FindStraightPath(Tile* from, Tile* to, T* sender)
	{
		if ((from->x == to->x && from->y == to->y) ||
			to->tileType == 1 || to->doorColor != -1 ||
			from == nullptr || to == nullptr)
			return nullptr;

		List<Tile*>* path = new List<Tile*>();
		from->pathIndex = 0;

		List<Tile*> openedCells;
		List<Tile*> closedCells;
		openedCells.Add(from);

		int iterationsLeft;
		if (sender != nullptr)
			iterationsLeft = ((Creature*)sender)->viewDistance;
		else iterationsLeft = mapSizeX * mapSizeY;

		Tile* cur = from;
		Tile* neighborTile;
		int curMinPathIndex = -1;
		int lastMinPathIndex = -1;
		while (/*CheckNeighborsToPathIndex(cur) && */iterationsLeft > 0)
		{
			neighborTile = GetTile(cur->x, cur->y + 1);
			if (neighborTile == to ||
				(cur->y + 1 < mapSizeY &&
					neighborTile != nullptr &&
					neighborTile->tileType != 1 &&
					neighborTile->doorColor == -1 &&
					neighborTile->pathIndex == -1 &&
					openedCells.Contains(neighborTile) == false))
			{
				neighborTile->pathIndex = cur->pathIndex + neighborTile->pathCost;
				openedCells.Add(neighborTile);
			}

			neighborTile = GetTile(cur->x, cur->y - 1);
			if (neighborTile == to ||
				(cur->y - 1 >= 0 &&
					neighborTile != nullptr &&
					neighborTile->tileType != 1 &&
					neighborTile->doorColor == -1 &&
					neighborTile->pathIndex == -1 &&
					openedCells.Contains(neighborTile) == false))
			{
				neighborTile->pathIndex = cur->pathIndex + neighborTile->pathCost;
				openedCells.Add(neighborTile);
			}

			neighborTile = GetTile(cur->x + 1, cur->y);
			if (neighborTile == to ||
				(cur->x + 1 < mapSizeX &&
					neighborTile != nullptr &&
					neighborTile->tileType != 1 &&
					neighborTile->doorColor == -1 &&
					neighborTile->pathIndex == -1 &&
					openedCells.Contains(neighborTile) == false))
			{
				neighborTile->pathIndex = cur->pathIndex + neighborTile->pathCost;
				openedCells.Add(neighborTile);
			}

			neighborTile = GetTile(cur->x - 1, cur->y);
			if (neighborTile == to ||
				(cur->x - 1 >= 0 &&
					neighborTile != nullptr &&
					neighborTile->tileType != 1 &&
					neighborTile->doorColor == -1 &&
					neighborTile->pathIndex == -1 &&
					openedCells.Contains(neighborTile) == false))
			{
				neighborTile->pathIndex = cur->pathIndex + neighborTile->pathCost;
				openedCells.Add(neighborTile);
			}

			closedCells.Add(cur);
			if (openedCells.Contains(to))
				break;

			curMinPathIndex = -1;
			for (int i = 0; i < openedCells.Count(); i++)
			{
				if (closedCells.Contains(openedCells[i]))
					continue;
				if (curMinPathIndex == -1 || curMinPathIndex > openedCells[i]->pathIndex)
				{
					cur = openedCells[i];
					curMinPathIndex = cur->pathIndex;
				}
			}
			if (curMinPathIndex == -1)
				break;
			if (curMinPathIndex > lastMinPathIndex)
				iterationsLeft--;
			lastMinPathIndex = curMinPathIndex;
		}

		if (openedCells.Contains(to) == false)
		{
			delete path;
			path = nullptr;
			for (int i = 0; i < openedCells.Count(); i++)
				(*openedCells[i]).pathIndex = -1;
			return nullptr;
		}

		Tile* toGo = nullptr;
		cur = to;
		int minNumber = cur->pathIndex;

		path->Add(to);
		while (cur != from)
		{
			neighborTile = GetTile(cur->x, cur->y + 1);
			if (neighborTile == from ||
				(cur->y + 1 < mapSizeY &&
					neighborTile != nullptr &&
					neighborTile->pathIndex != -1 &&
					neighborTile->pathIndex < minNumber))
			{
				toGo = neighborTile;
				minNumber = toGo->pathIndex;
			}

			neighborTile = GetTile(cur->x, cur->y - 1);
			if (neighborTile == from ||
				(cur->y - 1 >= 0 &&
					neighborTile != nullptr &&
					neighborTile->pathIndex != -1 &&
					neighborTile->pathIndex < minNumber))
			{
				toGo = neighborTile;
				minNumber = toGo->pathIndex;
			}

			neighborTile = GetTile(cur->x + 1, cur->y);
			if (neighborTile == from ||
				(cur->x + 1 < mapSizeX &&
					neighborTile != nullptr &&
					neighborTile->pathIndex != -1 &&
					neighborTile->pathIndex < minNumber))
			{
				toGo = neighborTile;
				minNumber = toGo->pathIndex;
			}

			neighborTile = GetTile(cur->x - 1, cur->y);
			if (neighborTile == from ||
				(cur->x - 1 >= 0 &&
					neighborTile != nullptr &&
					neighborTile->pathIndex != -1 &&
					neighborTile->pathIndex < minNumber))
			{
				toGo = neighborTile;
				minNumber = toGo->pathIndex;
			}

			if (toGo != from)
				path->Add(toGo);
			cur = toGo;
			toGo = nullptr;
		}

		for (int i = 0; i < openedCells.Count(); i++)
			(*openedCells[i]).pathIndex = -1;
		openedCells.RemoveList();
		return path;
	}

	/// <summary>
	/// returns true if at least one neighbor tile is passable and has pathIndex equals to -1
	/// </summary>
	bool CheckNeighborsToPathIndex(Tile* at)
	{
		Tile* t;
		return (
			((t = GetTile(at->x, at->y + 1)) != nullptr && t->tileType == 0 && t->GetTileObject() == nullptr && t->pathIndex == -1) ||
			((t = GetTile(at->x, at->y - 1)) != nullptr && t->tileType == 0 && t->GetTileObject() == nullptr && t->pathIndex == -1) ||
			((t = GetTile(at->x + 1, at->y)) != nullptr && t->tileType == 0 && t->GetTileObject() == nullptr && t->pathIndex == -1) ||
			((t = GetTile(at->x - 1, at->y)) != nullptr && t->tileType == 0 && t->GetTileObject() == nullptr && t->pathIndex == -1));
	}

	/// <summary>
	/// returns count of floor neighbor tiles
	/// </summary>
	int GetPassableNeighborsCount(Tile* at)
	{
		Tile* t;
		return 
			((t = GetTile(at->x, at->y + 1)) != nullptr && t->tileType == 0) +
			((t = GetTile(at->x, at->y - 1)) != nullptr && t->tileType == 0) +
			((t = GetTile(at->x + 1, at->y)) != nullptr && t->tileType == 0) +
			((t = GetTile(at->x - 1, at->y)) != nullptr && t->tileType == 0);
	}

	template<class T, class = typename enable_if<is_base_of<Creature, T>::value>::type>
	T* GetCreatureInFront(T *sender)
	{
		switch (((Creature*)sender)->facingDirection)
		{
		case (int)FacingDirection::Up:
			return (T*)GetTile(sender->x, sender->y - 1)->GetTileObject();
		case (int)FacingDirection::Down:
			return (T*)GetTile(sender->x, sender->y + 1)->GetTileObject();
		case (int)FacingDirection::Right:
			return (T*)GetTile(sender->x + 1, sender->y)->GetTileObject();
		case (int)FacingDirection::Left:
			return (T*)GetTile(sender->x - 1, sender->y)->GetTileObject();
		default:
			return nullptr;
		}
	}

	template<class T, class = typename enable_if<is_base_of<Creature, T>::value>::type>
	TileObject* GetTileObjectInFront(T* sender)
	{
		switch (((Creature*)sender)->facingDirection)
		{
		case (int)FacingDirection::Up:
			return GetTile(sender->x, sender->y - 1)->GetTileObject();
		case (int)FacingDirection::Down:
			return GetTile(sender->x, sender->y + 1)->GetTileObject();
		case (int)FacingDirection::Right:
			return GetTile(sender->x + 1, sender->y)->GetTileObject();
		case (int)FacingDirection::Left:
			return GetTile(sender->x - 1, sender->y)->GetTileObject();
		default:
			return nullptr;
		}
	}

	template<class T, class = typename enable_if<is_base_of<Creature, T>::value>::type>
	TileObject* GetTileObjectInDirection(Tile* tile, T* sender)
	{
		if (tile == nullptr)
			return nullptr;
		switch (((Creature*)sender)->facingDirection)
		{
		case (int)FacingDirection::Up:
			return GetTile(tile->x, tile->y - 1)->GetTileObject();
		case (int)FacingDirection::Down:
			return GetTile(tile->x, tile->y + 1)->GetTileObject();
		case (int)FacingDirection::Right:
			return GetTile(tile->x + 1, tile->y)->GetTileObject();
		case (int)FacingDirection::Left:
			return GetTile(tile->x - 1, tile->y)->GetTileObject();
		default:
			return nullptr;
		}
	}

	template<class T, class = typename enable_if<is_base_of<Creature, T>::value>::type>
	Tile* GetTileInFront(T *sender)
	{
		switch (((Creature*)sender)->facingDirection)
		{
		case (int)FacingDirection::Up:
			return GetTile(sender->x, sender->y - 1);
		case (int)FacingDirection::Down:
			return GetTile(sender->x, sender->y + 1);
		case (int)FacingDirection::Right:
			return GetTile(sender->x + 1, sender->y);
		case (int)FacingDirection::Left:
			return GetTile(sender->x - 1, sender->y);
		default:
			return nullptr;
		}
	}
};