#pragma once

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()

#include <SDL.h>
#include "Tilemap.h"
#include "Camera.h"
#include "MapController.h"
#include "GameController.h"
#include "GameInitializer.h"

class MapHandler sealed
{
private:
	MapHandler(void) {}
	MapHandler(const MapHandler& gc) {}
	MapHandler& operator = (MapHandler& gc) {}
	~MapHandler(void) 
	{
		delete minimapPosition;
		delete minimapSrc;
		SDL_DestroyTexture(minimapBackgroundTexture);
	}

	SDL_Point topLeftExploredTile;
	SDL_Point bottomRightExploredTile;
	int minimapSizeX;
	int minimapSizeY;
	SDL_Rect *minimapPosition;
	SDL_Rect *minimapSrc;

	SDL_Texture* minimapBackgroundTexture;
	vector<Tile*> visibleTiles;
public:
	static MapHandler& Instance()
	{
		static MapHandler instance;
		return instance;
	}

	vector<Tile*>& GetVisibleTiles()
	{
		return visibleTiles;
	}

	bool minimapIsShown;
	int minimapScale = 12;
	// max minimap size in tiles
	// int minimapMaxSize = 11;

	void Initialize()
	{
		minimapPosition = new SDL_Rect();
		minimapSrc = new SDL_Rect();
		topLeftExploredTile.x = topLeftExploredTile.y = -1;
		bottomRightExploredTile.x = bottomRightExploredTile.y = -1;

		auto surf = IMG_Load("textures/minimap_background.png");
		minimapBackgroundTexture = SDL_CreateTextureFromSurface(Initializer.renderer, surf);
		SDL_FreeSurface(surf);
	}
	void RecalculateVisibleLands(MapController *map, Tile* tileFrom, int viewDistance)
	{
		SDL_SetRenderTarget(Initializer.renderer, map->outOfSightLandsTexture);
		if (map->zoom < 1)
			SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
		SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, 0);
		SDL_SetRenderDrawBlendMode(Initializer.renderer, SDL_BLENDMODE_NONE);
		SDL_SetTextureBlendMode(map->outOfSightLandsTexture, SDL_BLENDMODE_BLEND);

		SDL_Rect tileScr =
		{
			6 * Initializer.TEXTURE_RES,
			0,
			Initializer.TEXTURE_RES,
			Initializer.TEXTURE_RES
		};
		SDL_Rect tileRect;
		for (auto i = visibleTiles.begin(); i != visibleTiles.end(); i++)
		{
			if (!(*i)->isVisible)
				continue;
			(*i)->isVisible = false;
			tileRect =
			{
				(*i)->x * Initializer.TEXTURE_RES,
				(*i)->y * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			};
			SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
		}
		visibleTiles.clear();

		bool visibleLandsChanged = false;

		Tile* curTile;

		List<Tile*> *markedTiles = new List<Tile*>();
		List<Tile*> *oldMarkedTiles = new List<Tile*>();
		List<Tile*> *newMarkedTiles = new List<Tile*>();
		Tile* neighborTile = tileFrom;

		markedTiles->Add(tileFrom);
		oldMarkedTiles->Add(tileFrom);
		tileFrom->isVisible = true;
		visibleTiles.push_back(tileFrom);
		int markedCellsCount;

		auto neighborTileExploring =
			[&]()
		{
			tileRect =
			{
				neighborTile->x * Initializer.TEXTURE_RES,
				neighborTile->y * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			};

			if (!neighborTile->isVisible)
				visibleLandsChanged = true;
			neighborTile->isVisible = true;
			visibleTiles.push_back(neighborTile);
			SDL_RenderFillRect(Initializer.renderer, &tileRect);
		};

		neighborTileExploring();
		while (viewDistance > 0)
		{
			viewDistance--;
			newMarkedTiles->RemoveList();
			markedCellsCount = oldMarkedTiles->Count();
			for (int i = 0; i < markedCellsCount; i++)
				newMarkedTiles->Add((*oldMarkedTiles)[i]);
			markedCellsCount = (*newMarkedTiles).Count();
			oldMarkedTiles->RemoveList();
			for (int i = 0; i < markedCellsCount; i++)
			{
				curTile = (*newMarkedTiles)[i];
				neighborTile = map->tilemap->GetTile(curTile->x, curTile->y + 1);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0 &&
					neighborTile->isExplored)
				{
					neighborTileExploring();
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = map->tilemap->GetTile(curTile->x, curTile->y - 1);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0 &&
					neighborTile->isExplored)
				{
					neighborTileExploring();
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = map->tilemap->GetTile(curTile->x + 1, curTile->y);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0 &&
					neighborTile->isExplored)
				{
					neighborTileExploring();
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = map->tilemap->GetTile(curTile->x - 1, curTile->y);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0 &&
					neighborTile->isExplored)
				{
					neighborTileExploring();
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}
			}
		}

		markedCellsCount = (*markedTiles).Count();
		for (int i = 0; i < markedCellsCount; i++)
		{
			curTile = (*markedTiles)[i];

			// light the walls too
			neighborTile = map->tilemap->GetTile(curTile->x, curTile->y + 1);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				neighborTile->isExplored)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x, curTile->y - 1);
			if (neighborTile != nullptr &&
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				neighborTile->isExplored)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x + 1, curTile->y);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				neighborTile->isExplored)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x - 1, curTile->y);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				neighborTile->isExplored)
				neighborTileExploring();

			neighborTile = map->tilemap->GetTile(curTile->x + 1, curTile->y + 1);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				neighborTile->isExplored)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x + 1, curTile->y - 1);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				neighborTile->isExplored)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x - 1, curTile->y + 1);
			if (neighborTile != nullptr &&
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				neighborTile->isExplored)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x - 1, curTile->y - 1);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				neighborTile->isExplored)
				neighborTileExploring();
		}

		delete markedTiles;
		delete oldMarkedTiles;
		delete newMarkedTiles;

		SDL_SetRenderTarget(Initializer.renderer, nullptr);
		if (map->zoom < 1)
			SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);

		RecalculateMinimap(map);
	}
	void RecalculateFogOfWar(MapController *map, Tile* tileFrom, int viewDistance)
	{
		bool fogOfWarChanged = false;

		Tile* curTile;
		SDL_Rect tileRect;
		SDL_Rect tileScr;

		/*SDL_SetRenderTarget(Initializer.renderer, map->fogOfWarTexture);
		if (map->zoom < 1)
			SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
		SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, 0);
		SDL_SetRenderDrawBlendMode(Initializer.renderer, SDL_BLENDMODE_NONE);
		SDL_SetTextureBlendMode(map->fogOfWarTexture, SDL_BLENDMODE_BLEND);*/

		List<Tile*> *markedTiles = new List<Tile*>();
		List<Tile*> *oldMarkedTiles = new List<Tile*>();
		List<Tile*> *newMarkedTiles = new List<Tile*>();
		Tile* neighborTile = tileFrom;

		markedTiles->Add(tileFrom);
		oldMarkedTiles->Add(tileFrom);
		int markedCellsCount;

		auto neighborTileExploring =
			[&]()
			{
				tileRect =
				{
					neighborTile->x * Initializer.TEXTURE_RES,
					neighborTile->y * Initializer.TEXTURE_RES,
					Initializer.TEXTURE_RES,
					Initializer.TEXTURE_RES
				};
				if (neighborTile->tileType == 0 && neighborTile->zoneNumber == map->biggestZoneNumber)
				{
					if (!neighborTile->isExplored)
					{
						//if (neighborTile->doorIndex == -1)
						{
							if (rand() % 100 > 10)
								tileScr =
								{
									rand() % 5 * Initializer.TEXTURE_RES,
									0,
									Initializer.TEXTURE_RES,
									Initializer.TEXTURE_RES
								};
							else
								tileScr =
								{
									rand() % 5 * Initializer.TEXTURE_RES,
									Initializer.TEXTURE_RES,
									Initializer.TEXTURE_RES,
									Initializer.TEXTURE_RES
								};
						}
						/*else
							tileScr =
							{
								(7 + neighborTile->doorIndex) * Initializer.TEXTURE_RES,
								Initializer.TEXTURE_RES,
								Initializer.TEXTURE_RES,
								Initializer.TEXTURE_RES
							};*/
						Controller.sessionExploredTilesCount++;
						SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);

						// render grid
						tileScr =
						{
							5 * Initializer.TEXTURE_RES,
							Initializer.TEXTURE_RES,
							Initializer.TEXTURE_RES,
							Initializer.TEXTURE_RES
						};
						SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
					}
				}
				else if (neighborTile->tileType == 1 ||
					(neighborTile->tileType == 0 && neighborTile->zoneNumber != map->biggestZoneNumber))
				{
					Tile* tempTile;
					unsigned char tileByte = 0;
					for (int _x = neighborTile->x - 1; _x <= neighborTile->x + 1; _x++)
						for (int _y = neighborTile->y - 1; _y <= neighborTile->y + 1; _y++)
						{
							if (_x == neighborTile->x && _y == neighborTile->y)
								continue;
							tempTile = map->tilemap->GetTile(_x, _y);
							if (tempTile != nullptr && tempTile->isExplored &&
								(tempTile->generationTileType == Tile::GenerationTileType::TrueFloor ||
									tempTile->generationTileType == Tile::GenerationTileType::Floor))
								// translating matrix index to tile byte from 1 to 8 (from 0 to 7 actually: as powers of 2)
								// example:
								// () means floor
								// (1) (4)  6
								//  2   -  (7)  --> 1 1 0 0 1 1 0 1 (output) -- warning! output byte is straight (right bit is actually 1, next is 2 etc.)
								// (3)  5  (8)
								tileByte |= (int)pow(2, (_x - (neighborTile->x - 1)) * 3 + (_y - (neighborTile->y - 1)) -
									// subtract 1 if index is 5 or more (due to middle "missing" index)
									((_x - (neighborTile->x - 1)) * 3 + (_y - (neighborTile->y - 1)) > 4));
						}
					tileScr =
					{
						// cut off the left part:	0110 0010 --> 0000 0010 = 2 column
						(tileByte & (1 + 2 + 4 + 8)) * Initializer.TEXTURE_RES,
						// cut off the right part:	0110 0010 --> 0000 0110 = 6 row
						(tileByte >> 4) * Initializer.TEXTURE_RES,
						Initializer.TEXTURE_RES,
						Initializer.TEXTURE_RES
					};
					SDL_RenderCopy(Initializer.renderer, Initializer.tilesetWall, &tileScr, &tileRect);
				}
				else
				{
					if (!neighborTile->isExplored)
					{
						tileScr =
						{
							0,
							0,
							Initializer.TEXTURE_RES,
							Initializer.TEXTURE_RES
						};
						SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
						tileScr =
						{
							neighborTile->doorColor * Initializer.TEXTURE_RES,
							2 * Initializer.TEXTURE_RES,
							Initializer.TEXTURE_RES,
							Initializer.TEXTURE_RES
						};
						SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
					}
				}

				if (!neighborTile->isExplored)
					fogOfWarChanged = true;
				if (neighborTile->x < topLeftExploredTile.x ||
					topLeftExploredTile.x == -1)
					topLeftExploredTile.x = neighborTile->x;
				if (neighborTile->y < topLeftExploredTile.y ||
					topLeftExploredTile.y == -1)
					topLeftExploredTile.y = neighborTile->y;
				if (neighborTile->x > bottomRightExploredTile.x ||
					bottomRightExploredTile.x == -1)
					bottomRightExploredTile.x = neighborTile->x;
				if (neighborTile->y > bottomRightExploredTile.y ||
					bottomRightExploredTile.y == -1)
					bottomRightExploredTile.y = neighborTile->y;
				neighborTile->isExplored = true;
				//SDL_RenderFillRect(Initializer.renderer, &tileRect);
			};

		SDL_SetRenderTarget(Initializer.renderer, map->mapTexture);
		if (map->zoom < 1)
			SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
		neighborTileExploring();
		while (viewDistance > 0)
		{
			viewDistance--;
			newMarkedTiles->RemoveList();
			markedCellsCount = oldMarkedTiles->Count();
			for (int i = 0; i < markedCellsCount; i++)
				newMarkedTiles->Add((*oldMarkedTiles)[i]);
			markedCellsCount = (*newMarkedTiles).Count();
			oldMarkedTiles->RemoveList();
			for (int i = 0; i < markedCellsCount; i++)
			{
				curTile = (*newMarkedTiles)[i];
				neighborTile = map->tilemap->GetTile(curTile->x, curTile->y + 1);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0)
				{
					neighborTileExploring();
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = map->tilemap->GetTile(curTile->x, curTile->y - 1);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0)
				{
					neighborTileExploring();
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = map->tilemap->GetTile(curTile->x + 1, curTile->y);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0)
				{
					neighborTileExploring();
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = map->tilemap->GetTile(curTile->x - 1, curTile->y);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0)
				{
					neighborTileExploring();
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}
			}
		}
		
		// 0 - right-up
		// 1 - right-down
		// 2 - left-down
		// 3 - left-up
		auto checkWall =
			[&](int direction)
			{
				Tile* t;
				switch (direction)
				{
				case 0:
					t = map->tilemap->GetTile(neighborTile->x - 1, neighborTile->y);
					if (t == nullptr || !t->isExplored)
						return false;
					t = map->tilemap->GetTile(neighborTile->x, neighborTile->y + 1);
					if (t == nullptr || !t->isExplored)
						return false;
					return true;
				case 1:
					t = map->tilemap->GetTile(neighborTile->x - 1, neighborTile->y);
					if (t == nullptr || !t->isExplored)
						return false;
					t = map->tilemap->GetTile(neighborTile->x, neighborTile->y - 1);
					if (t == nullptr || !t->isExplored)
						return false;
					return true;
				case 2:
					t = map->tilemap->GetTile(neighborTile->x + 1, neighborTile->y);
					if (t == nullptr || !t->isExplored)
						return false;
					t = map->tilemap->GetTile(neighborTile->x, neighborTile->y - 1);
					if (t == nullptr || !t->isExplored)
						return false;
					return true;
				case 3: 
					t = map->tilemap->GetTile(neighborTile->x + 1, neighborTile->y);
					if (t == nullptr || !t->isExplored)
						return false;
					t = map->tilemap->GetTile(neighborTile->x, neighborTile->y + 1);
					if (t == nullptr || !t->isExplored)
						return false;
					return true;
				default: return false;
				}
			};
		markedCellsCount = (*markedTiles).Count();
		for (int i = 0; i < markedCellsCount; i++)
		{
			curTile = (*markedTiles)[i];

			// light the walls too
			neighborTile = map->tilemap->GetTile(curTile->x, curTile->y + 1);
			if (neighborTile != nullptr && markedTiles->Contains(neighborTile) == false && neighborTile->tileType != 0)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x, curTile->y - 1);
			if (neighborTile != nullptr && markedTiles->Contains(neighborTile) == false && neighborTile->tileType != 0)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x + 1, curTile->y);
			if (neighborTile != nullptr && markedTiles->Contains(neighborTile) == false && neighborTile->tileType != 0)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x - 1, curTile->y);
			if (neighborTile != nullptr && markedTiles->Contains(neighborTile) == false && neighborTile->tileType != 0)
				neighborTileExploring();

			neighborTile = map->tilemap->GetTile(curTile->x + 1, curTile->y + 1);
			if (neighborTile != nullptr && 
				neighborTile->tileType != 0 &&
				markedTiles->Contains(neighborTile) == false &&
				checkWall(1)
				/*map->tilemap->CheckExploredNeighbor(neighborTile)*/)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x + 1, curTile->y - 1);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				checkWall(0)
				/*map->tilemap->CheckExploredNeighbor(neighborTile)*/)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x - 1, curTile->y + 1);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				checkWall(2)
				/*map->tilemap->CheckExploredNeighbor(neighborTile)*/)
				neighborTileExploring();
			neighborTile = map->tilemap->GetTile(curTile->x - 1, curTile->y - 1);
			if (neighborTile != nullptr && 
				markedTiles->Contains(neighborTile) == false && 
				neighborTile->tileType != 0 &&
				checkWall(3)
				/*map->tilemap->CheckExploredNeighbor(neighborTile)*/)
				neighborTileExploring();
		}

		markedTiles->RemoveList();
		oldMarkedTiles->RemoveList();
		newMarkedTiles->RemoveList();
		delete markedTiles;
		delete oldMarkedTiles;
		delete newMarkedTiles;

		SDL_SetRenderTarget(Initializer.renderer, nullptr);
		if (map->zoom < 1)
			SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);

		RecalculateMinimap(map);
	}
	void RecalculateMinimap(MapController *map)
	{
		Uint32 pixelFormat;
		SDL_QueryTexture(Initializer.tilesetFloor, &pixelFormat, NULL, NULL, NULL);

		minimapSizeX = minimapScale * (bottomRightExploredTile.x - topLeftExploredTile.x + 1);
		minimapSizeY = minimapScale * (bottomRightExploredTile.y - topLeftExploredTile.y + 1);

		SDL_DestroyTexture(map->minimapTexture);
		map->minimapTexture = SDL_CreateTexture(Initializer.renderer, pixelFormat,
			SDL_TEXTUREACCESS_TARGET, minimapSizeX, minimapSizeY);

		Tile* curTile;
		SDL_Rect tileScr;
		SDL_Rect tileRect;
		Creature* curCreature;

		// fill the minimap with transparent color
		SDL_SetRenderTarget(Initializer.renderer, map->minimapTexture);
		SDL_SetRenderDrawBlendMode(Initializer.renderer, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(map->minimapTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(Initializer.renderer, 255, 255, 255, 0);
		SDL_RenderFillRect(Initializer.renderer, nullptr);
		for (int x = 0; x < map->mapSizeX; x++)
			for (int y = 0; y < map->mapSizeY; y++)
			{
				curTile = map->tilemap->GetTile(x, y);
				if (curTile != nullptr && curTile->isExplored)
				{
					tileRect =
					{
						abs(x - topLeftExploredTile.x) * minimapScale,
						abs(y - topLeftExploredTile.y) * minimapScale,
						minimapScale,
						minimapScale
					};
					if (curTile->generationTileType == Tile::GenerationTileType::TrueWall ||
						curTile->generationTileType == Tile::GenerationTileType::Wall)
					{
						tileScr =
						{
							7 * Initializer.TEXTURE_RES,
							0,
							Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES
						};
						SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
					}
					else if (curTile->generationTileType == Tile::GenerationTileType::TrueFloor ||
						curTile->generationTileType == Tile::GenerationTileType::Floor)
					{
						if (curTile->isVisible)
							tileScr =
						{
							7 * Initializer.TEXTURE_RES + Initializer.MINIMAP_TILE_RES,
							0,
							Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES
						};
						else
							tileScr =
						{
							7 * Initializer.TEXTURE_RES + 4 * Initializer.MINIMAP_TILE_RES,
							0,
							Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES
						};
						SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
					}

					if (curTile->doorColor != -1)
					{
						tileScr =
						{
							7 * Initializer.TEXTURE_RES + curTile->doorColor * Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES
						};
						SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
					}

					if (curTile->tileItem != nullptr)
					{
						if (curTile->isVisible)
							tileScr =
						{
							9 * Initializer.TEXTURE_RES + Initializer.MINIMAP_TILE_RES,
							0,
							Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES
						};
						else
							tileScr =
						{
							9 * Initializer.TEXTURE_RES + Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES,
							Initializer.MINIMAP_TILE_RES
						};
						SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
					}

					if (curTile->GetTileObject() != nullptr)
					{
						curCreature = dynamic_cast<Creature*>(curTile->GetTileObject());
						if (curCreature != nullptr)
						{
							if (curTile->isVisible)
							{
								if (curCreature->isUnderPlayerControl)
								{
									tileScr =
									{
										7 * Initializer.TEXTURE_RES + 2 * Initializer.MINIMAP_TILE_RES,
										0,
										Initializer.MINIMAP_TILE_RES,
										Initializer.MINIMAP_TILE_RES
									};
								}
								else
								{
									tileScr =
									{
										7 * Initializer.TEXTURE_RES + 3 * Initializer.MINIMAP_TILE_RES,
										0,
										Initializer.MINIMAP_TILE_RES,
										Initializer.MINIMAP_TILE_RES
									};
								}
								SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
							}
						}
						else if (curTile->GetTileObject()->canInteractWith)
						{
							tileScr =
							{
								10 * Initializer.TEXTURE_RES,
								0,
								Initializer.MINIMAP_TILE_RES,
								Initializer.MINIMAP_TILE_RES
							};
							SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
						}
					}
				}
			}

		SDL_SetRenderTarget(Initializer.renderer, nullptr);
		SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, 255);
		if (map->zoom < 1)
			SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
	}

	void ShowMinimap(MapController *map, Creature* player)
	{
		if (!minimapIsShown)
			return;
		/*minimapPosition->x = Initializer.SCREEN_WIDTH / 2
			- ((player->x - topLeftExploredTile.x) * minimapScale);
		minimapPosition->y = Initializer.SCREEN_HEIGHT / 2
			- ((player->y - topLeftExploredTile.y) * minimapScale);*/
		//minimapPosition->x = (Initializer.SCREEN_WIDTH - 300) / 2;
		//minimapPosition->y = (Initializer.SCREEN_HEIGHT - 300) / 2;
		//minimapPosition->w = min(minimapMaxSize * minimapScale, minimapSizeX - minimapMaxSize * minimapScale);//minimapSizeX;
		//minimapPosition->h = min(minimapMaxSize * minimapScale, minimapSizeY - minimapMaxSize * minimapScale); //minimapSizeY;

		SDL_RenderSetScale(Initializer.renderer, 1, 1);
		minimapPosition->x = (Initializer.SCREEN_WIDTH - 950) / 2;
		minimapPosition->y = (Initializer.SCREEN_HEIGHT - 950) / 2;
		minimapPosition->w = 950;
		minimapPosition->h = 950;
		SDL_RenderCopy(Initializer.renderer, minimapBackgroundTexture, nullptr, minimapPosition);

		minimapPosition->x = (Initializer.SCREEN_WIDTH - minimapSizeX) / 2;
		minimapPosition->y = (Initializer.SCREEN_HEIGHT - minimapSizeY) / 2;
		/*minimapPosition->x = Initializer.SCREEN_WIDTH / 2
			- ((player->x - topLeftExploredTile.x) * minimapScale);
		minimapPosition->y = Initializer.SCREEN_HEIGHT / 2
			- ((player->y - topLeftExploredTile.y) * minimapScale);*/
		minimapPosition->w = minimapSizeX;
		minimapPosition->h = minimapSizeY;
		
		//int srcStart;
		//if (minimapSizeX < minimapMaxSize * minimapScale)
		//{
		//	srcStart = (player->x - topLeftExploredTile.x) * minimapScale;
		//	minimapSrc->x =
		//		clamp(srcStart,
		//			0,//minimapMaxSize * minimapScale,
		//			minimapSizeX); //(player->y - topLeftExploredTile.y) * minimapScale;
		//	minimapPosition->w = minimapSrc->w = minimapSizeX - srcStart;
		//}
		//else
		//{
		//	srcStart = (player->x - topLeftExploredTile.x - minimapMaxSize / 2) * minimapScale;
		//	minimapSrc->x =
		//		clamp(srcStart,
		//			0,//minimapMaxSize * minimapScale,
		//			minimapSizeX - minimapMaxSize * minimapScale); //(player->y - topLeftExploredTile.y) * minimapScale;
		//	if (srcStart + minimapMaxSize * minimapScale <= minimapSizeX)
		//		minimapPosition->w = minimapSrc->w = minimapMaxSize * minimapScale;
		//	else minimapPosition->w = minimapSrc->w = minimapSizeX - srcStart;
		//}
		//if (minimapSizeY < minimapMaxSize * minimapScale)
		//{
		//	srcStart = (player->y - topLeftExploredTile.y) * minimapScale;
		//	minimapSrc->y =
		//		clamp(srcStart,
		//			0,//minimapMaxSize * minimapScale,
		//			minimapSizeY); //(player->y - topLeftExploredTile.y) * minimapScale;
		//	minimapPosition->h = minimapSrc->h = minimapSizeY - srcStart;
		//}
		//else
		//{
		//	srcStart = (player->y - topLeftExploredTile.y - minimapMaxSize / 2) * minimapScale;
		//	minimapSrc->y =
		//		clamp(srcStart,
		//			0,//minimapMaxSize * minimapScale,
		//			minimapSizeY - minimapMaxSize * minimapScale); //(player->y - topLeftExploredTile.y) * minimapScale;
		//	if (srcStart + minimapMaxSize * minimapScale <= minimapSizeY)
		//		minimapPosition->h = minimapSrc->h = minimapMaxSize * minimapScale;
		//	else minimapPosition->h = minimapSrc->h = minimapSizeY - srcStart;
		//}
		//srcStart = (player->y - topLeftExploredTile.y - minimapMaxSize / 2) * minimapScale;
		//minimapSrc->y =
		//	clamp(srcStart,
		//		0,//minimapMaxSize * minimapScale,
		//		minimapSizeY - minimapMaxSize * minimapScale); //(player->y - topLeftExploredTile.y) * minimapScale;
		//minimapSrc->h = 
		//	minimapSizeY < minimapMaxSize * minimapScale 
		//	? minimapSizeY 
		//	: min(minimapMaxSize * minimapScale, minimapSizeY - minimapMaxSize * minimapScale);

		//SDL_RenderCopy(Initializer.renderer, map->minimapTexture, minimapSrc, minimapPosition);
		SDL_RenderCopy(Initializer.renderer, map->minimapTexture, nullptr, minimapPosition);
		SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
	}
};