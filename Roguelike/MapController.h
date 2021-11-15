#pragma once

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()

#include <SDL.h>
#include "Tilemap.h"
#include "Camera.h"
#include "PerlinNoise.hpp"

using namespace std;

class MapController final
{
private:
	Tile* tileForPlayerToSpawn;
	vector<Tile*> tilesForCreaturesToSpawn;
	Tile* curTile;
public:
	vector<Tile*>* behindDoorTiles;
	vector<Tile*> doorTiles;
	List<Tile*>* biggestZone;
	int biggestZoneNumber;
	int explorableTilesCount;

	Tilemap *tilemap;
	const int mapSizeX = 50;
	const int mapSizeY = 50;
	Camera camera = Camera();

	double zoom = 6;
	SDL_Texture *mapTexture;
	SDL_Texture *outOfSightLandsTexture;
	SDL_Texture *minimapTexture;

	SDL_Rect *mapPosition = new SDL_Rect();
	SDL_Rect *mapScreen = new SDL_Rect();
	SDL_Rect *fogOfWarPosition = new SDL_Rect();

	~MapController()
	{
		SDL_DestroyTexture(mapTexture);
		SDL_DestroyTexture(outOfSightLandsTexture);
		SDL_DestroyTexture(minimapTexture);
		delete mapPosition, mapScreen, fogOfWarPosition, tilemap;
		delete biggestZone;
	}

	List<Tile*>* SeparateZones(Tile* tileFrom, int zoneNumber, int viewDistance)
	{
		Tile* curTile;

		List<Tile*> *markedTiles = new List<Tile*>();
		List<Tile*> *oldMarkedTiles = new List<Tile*>();
		List<Tile*> *newMarkedTiles = new List<Tile*>();
		Tile* neighborTile = tileFrom;

		markedTiles->Add(tileFrom);
		oldMarkedTiles->Add(tileFrom);
		int markedCellsCount;

		neighborTile->zoneNumber = zoneNumber;
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
				neighborTile = tilemap->GetTile(curTile->x, curTile->y + 1);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0 &&
					neighborTile->zoneNumber == -1)
				{
					/*if (viewDistance == 0)
						neighborTile->tileType = 2;*/
					neighborTile->zoneNumber = zoneNumber;
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = tilemap->GetTile(curTile->x, curTile->y - 1);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0 &&
					neighborTile->zoneNumber == -1)
				{
					/*if (viewDistance == 0)
						neighborTile->tileType = 2;*/
					neighborTile->zoneNumber = zoneNumber;
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = tilemap->GetTile(curTile->x + 1, curTile->y);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0 &&
					neighborTile->zoneNumber == -1)
				{
					/*if (viewDistance == 0)
						neighborTile->tileType = 2;*/
					neighborTile->zoneNumber = zoneNumber;
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}

				neighborTile = tilemap->GetTile(curTile->x - 1, curTile->y);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0 &&
					neighborTile->zoneNumber == -1)
				{
					/*if (viewDistance == 0)
						neighborTile->tileType = 2;*/
					neighborTile->zoneNumber = zoneNumber;
					oldMarkedTiles->Add(neighborTile);
					markedTiles->Add(neighborTile);
				}
			}
		}

		oldMarkedTiles->RemoveList();
		newMarkedTiles->RemoveList();
		delete oldMarkedTiles;
		delete newMarkedTiles;
		return markedTiles;
	}

	void PrepareMap(void)
	{
		mapPosition->x = 0;
		mapPosition->y = 0;
		mapPosition->w = mapScreen->w = Initializer.SCREEN_WIDTH;
		mapPosition->h = mapScreen->h = Initializer.SCREEN_HEIGHT;
		fogOfWarPosition->w = (int)(Initializer.TEXTURE_RES * mapSizeX * zoom);
		fogOfWarPosition->h = (int)(Initializer.TEXTURE_RES * mapSizeY * zoom);

		tilemap = new Tilemap(mapSizeX, mapSizeY);
		Uint32 pixelFormat;
		SDL_QueryTexture(Initializer.tilesetFloor, &pixelFormat, NULL, NULL, NULL);

		mapTexture = SDL_CreateTexture(Initializer.renderer, pixelFormat,
			SDL_TEXTUREACCESS_TARGET,
			(int)(Initializer.TEXTURE_RES * mapSizeX * zoom),
			(int)(Initializer.TEXTURE_RES * mapSizeY * zoom));
		/*fogOfWarTexture = SDL_CreateTexture(Initializer.renderer, pixelFormat,
			SDL_TEXTUREACCESS_TARGET,
			(int)(Initializer.TEXTURE_RES * mapSizeX * zoom),
			(int)(Initializer.TEXTURE_RES * mapSizeY * zoom));*/
		outOfSightLandsTexture = SDL_CreateTexture(Initializer.renderer, pixelFormat,
			SDL_TEXTUREACCESS_TARGET,
			(int)(Initializer.TEXTURE_RES * mapSizeX * zoom),
			(int)(Initializer.TEXTURE_RES * mapSizeY * zoom));
		minimapTexture = SDL_CreateTexture(Initializer.renderer, pixelFormat,
			SDL_TEXTUREACCESS_TARGET,
			(int)(Initializer.MINIMAP_TILE_RES * mapSizeX),
			(int)(Initializer.MINIMAP_TILE_RES * mapSizeY));

		// SDL_Rect tileScr = { (rand() % 2) * Initializer.TEXTURE_RES, (rand() % 2) * Initializer.TEXTURE_RES, Initializer.TEXTURE_RES, Initializer.TEXTURE_RES };
		SDL_Rect tileScr =
		{
			Initializer.TEXTURE_RES,
			0,
			Initializer.TEXTURE_RES,
			Initializer.TEXTURE_RES
		};
		SDL_Rect tileRect;

		SDL_SetRenderTarget(Initializer.renderer, mapTexture);
		if (zoom < 1)
			SDL_RenderSetScale(Initializer.renderer, (float)zoom, (float)zoom);
		Tile *curTile = nullptr;

		const Noise::PerlinNoise perlin(rand());
		const double frequency = 12.0;
		const int32_t octaves = 8;
		const double fx = mapSizeX / frequency;
		const double fy = mapSizeY / frequency;
		double noiseValue;

		int wallsCounter;
		Tile* curSpaceTile;
		vector<Tile*> spaceTiles;
		bool isSpaceTile;
		int curX, curY;

		//double loadingTick = 100. / ((mapSizeX + 5.) * (mapSizeY + 5.));
		for (int x = 0; x < mapSizeX + 5; x++)
			for (int y = 0; y < mapSizeY + 5; y++)
			{
				//loadingPercent += loadingTick;
				curTile = tilemap->GetTile(x, y);
				if (curTile != nullptr)
				{
					if (x == 0 || y == 0)
					{
						curTile->generationTileType = Tile::GenerationTileType::TrueWall;
						continue;
					}
					else if (x == mapSizeX - 1 || y == mapSizeY - 1)
						curTile->generationTileType = Tile::GenerationTileType::TrueWall;
					else
					{
						noiseValue = perlin.accumulatedOctaveNoise2D_0_1(x / fx, y / fy, octaves);
						if (noiseValue > 0.78)
							curTile->generationTileType = Tile::GenerationTileType::Wall;
						else if (noiseValue > 0.51)
							curTile->generationTileType = Tile::GenerationTileType::TrueFloor;
						else if (noiseValue > 0.48)
							curTile->generationTileType = Tile::GenerationTileType::Wall;
						else if (noiseValue > 0.44)
							curTile->generationTileType = (Tile::GenerationTileType)(rand() % 2 + 2);
						else
							curTile->generationTileType = Tile::GenerationTileType::Wall;
					}
				}

				if (x > 0 && y > 0)
				{
					// first cellular automata iteration
					wallsCounter = 0;
					curX = x - 1;
					curY = y - 1;
					curTile = tilemap->GetTile(curX, curY);
					if (curTile != nullptr && curTile->generationTileType != Tile::GenerationTileType::Unset)
					{
						for (int _x = curX - 1; _x <= curX + 1; _x++)
							for (int _y = curY - 1; _y <= curY + 1; _y++)
								if (_x != curX || _y != curY)
								{
									curSpaceTile = tilemap->GetTile(_x, _y);
									if (tilemap->GetTile(_x, _y) == nullptr ||
										curSpaceTile->generationTileType == Tile::GenerationTileType::TrueWall ||
										curSpaceTile->generationTileType == Tile::GenerationTileType::Wall ||
										curSpaceTile->generationTileType == Tile::GenerationTileType::Unset)
										wallsCounter++;
								}
						if (curTile->generationTileType == Tile::GenerationTileType::Floor)
						{
							if (wallsCounter >= 5)
								curTile->generationTileType = Tile::GenerationTileType::Wall;
						}
						else if (curTile->generationTileType == Tile::GenerationTileType::Wall)
							if (wallsCounter < 4)
								curTile->generationTileType = Tile::GenerationTileType::Floor;
					}

					if (x > 1 && y > 1)
					{
						// second cellular automata iteration
						wallsCounter = 0;
						curX = x - 2;
						curY = y - 2;
						curTile = tilemap->GetTile(curX, curY);
						if (curTile != nullptr && curTile->generationTileType != Tile::GenerationTileType::Unset)
						{
							for (int _x = curX - 1; _x <= curX + 1; _x++)
								for (int _y = curY - 1; _y <= curY + 1; _y++)
									if (_x != curX || _y != curY)
									{
										curSpaceTile = tilemap->GetTile(_x, _y);
										if (tilemap->GetTile(_x, _y) == nullptr ||
											curSpaceTile->generationTileType == Tile::GenerationTileType::TrueWall ||
											curSpaceTile->generationTileType == Tile::GenerationTileType::Wall ||
											curSpaceTile->generationTileType == Tile::GenerationTileType::Unset)
											wallsCounter++;
									}
							if (curTile->generationTileType == Tile::GenerationTileType::Floor)
							{
								if (wallsCounter >= 5)
									curTile->generationTileType = Tile::GenerationTileType::Wall;
							}
							else if (curTile->generationTileType == Tile::GenerationTileType::Wall)
								if (wallsCounter < 4)
									curTile->generationTileType = Tile::GenerationTileType::Floor;
						}

						if (x > 2 && y > 2)
						{
							// prevent too big empty rooms
							curX = x - 3;
							curY = y - 3;
							isSpaceTile = true;
							curSpaceTile = tilemap->GetTile(curX, curY);
							if (curSpaceTile != nullptr)
							{
								if (!(curSpaceTile->generationTileType == Tile::GenerationTileType::Wall ||
									curSpaceTile->generationTileType == Tile::GenerationTileType::TrueWall))
								{
									for (int _x = curX - 3; _x < curX + 3; _x++)
									{
										for (int _y = curY - 3; _y < curY + 3; _y++)
										{
											if ((curX - _x) * (curX - _x) + (curY - _y) * (curY - _y) > 9)
												continue;
											curTile = tilemap->GetTile(_x, _y);
											if (curTile != nullptr)
												if (curTile->generationTileType == Tile::GenerationTileType::Wall ||
													curTile->generationTileType == Tile::GenerationTileType::TrueWall)
												{
													isSpaceTile = false;
													break;
												}
										}
										if (!isSpaceTile) break;
									}
									if (isSpaceTile)
										spaceTiles.push_back(curSpaceTile);
								}
							}

							if (x > 3 && y > 3)
							{
								curX = x - 4;
								curY = y - 4;
								curTile = tilemap->GetTile(curX, curY);
								if (curTile != nullptr)
								{
									curTile->tileType = 0;
									if (curTile->generationTileType == Tile::GenerationTileType::TrueWall ||
										curTile->generationTileType == Tile::GenerationTileType::Wall)
										curTile->tileType = 1;

									tileScr =
									{
										6 * Initializer.TEXTURE_RES,
										0,
										Initializer.TEXTURE_RES,
										Initializer.TEXTURE_RES
									};
									SDL_SetRenderTarget(Initializer.renderer, outOfSightLandsTexture);
									if (zoom < 1)
										SDL_RenderSetScale(Initializer.renderer, (float)zoom, (float)zoom);
									SDL_SetRenderDrawBlendMode(Initializer.renderer, SDL_BLENDMODE_BLEND);
									SDL_SetTextureBlendMode(outOfSightLandsTexture, SDL_BLENDMODE_BLEND);
									SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
								}
							}

						}
					}
				}
			}
		for (auto i = spaceTiles.begin(); i != spaceTiles.end(); i++)
		{
			(*i)->generationTileType = Tile::GenerationTileType::Wall;
			(*i)->tileType = 1;
		}
		spaceTiles.clear();

		SDL_SetRenderTarget(Initializer.renderer, nullptr);
		if (zoom < 1)
			SDL_RenderSetScale(Initializer.renderer, (float)zoom, (float)zoom);

		int zoneNumber = 0;
		for (int x = 0; x < mapSizeX; x++)
			for (int y = 0; y < mapSizeY; y++)
			{
				curTile = tilemap->GetTile(x, y);
				if (curTile != nullptr && curTile->zoneNumber == -1 && curTile->tileType == 0)
				{
					auto newZone = SeparateZones(curTile, zoneNumber, mapSizeX * mapSizeY);
					if (biggestZone == nullptr || newZone->Count() > biggestZone->Count())
					{
						biggestZone = newZone;
						biggestZoneNumber = zoneNumber;
					}
					else delete newZone;
					zoneNumber++;
				}
			}

		int safeDistanceFromSpawn = 6;
		List<Tile*> *oldMarkedTiles = new List<Tile*>();
		List<Tile*> *newMarkedTiles = new List<Tile*>();
		while (tilemap->GetPassableNeighborsCount(tileForPlayerToSpawn = (*biggestZone)[rand() % biggestZone->Count()]) < 3);
		Tile* neighborTile = tileForPlayerToSpawn;

		oldMarkedTiles->Add(tileForPlayerToSpawn);
		int markedCellsCount;

		neighborTile->canSpawnCreature = false;
		while (safeDistanceFromSpawn > 0)
		{
			safeDistanceFromSpawn--;
			newMarkedTiles->RemoveList();
			markedCellsCount = oldMarkedTiles->Count();
			for (int i = 0; i < markedCellsCount; i++)
				newMarkedTiles->Add((*oldMarkedTiles)[i]);
			markedCellsCount = (*newMarkedTiles).Count();
			oldMarkedTiles->RemoveList();
			for (int i = 0; i < markedCellsCount; i++)
			{
				curTile = (*newMarkedTiles)[i];
				neighborTile = tilemap->GetTile(curTile->x, curTile->y + 1);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0)
				{
					neighborTile->canSpawnCreature = false;
					oldMarkedTiles->Add(neighborTile);
				}

				neighborTile = tilemap->GetTile(curTile->x, curTile->y - 1);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0)
				{
					neighborTile->canSpawnCreature = false;
					oldMarkedTiles->Add(neighborTile);
				}

				neighborTile = tilemap->GetTile(curTile->x + 1, curTile->y);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0)
				{
					neighborTile->canSpawnCreature = false;
					oldMarkedTiles->Add(neighborTile);
				}

				neighborTile = tilemap->GetTile(curTile->x - 1, curTile->y);
				if (neighborTile != nullptr &&
					!oldMarkedTiles->Contains(neighborTile) &&
					neighborTile->tileType == 0)
				{
					neighborTile->canSpawnCreature = false;
					oldMarkedTiles->Add(neighborTile);
				}
			}
		}

		delete oldMarkedTiles;
		delete newMarkedTiles;

		for (int i = 0; i < biggestZone->Count(); i++)
		{
			if ((*biggestZone)[i]->canSpawnCreature/* &&
				tilemap->GetPassableNeighborsCount((*biggestZone)[i]) >= 2*/)
			{
				for (int x = (*biggestZone)[i]->x - 2; x <= (*biggestZone)[i]->x + 2; x++)
					for (int y = (*biggestZone)[i]->y - 2; y <= (*biggestZone)[i]->y + 2; y++)
						if ((curTile = tilemap->GetTile(x, y)) != nullptr)
							curTile->canSpawnCreature = false;
				tilesForCreaturesToSpawn.push_back((*biggestZone)[i]);
			}
		}

		bool breakCycle;
		vector<int> possibleDoorTypes = { 0, 1, 2, 3, 4 };
		behindDoorTiles = new vector<Tile*>[5];
		int doorsCount = 0;
		for (int x = 0; x < mapSizeX && possibleDoorTypes.size() > 0 && doorsCount < 3; x++)
			for (int y = 0; y < mapSizeY && possibleDoorTypes.size() > 0 && doorsCount < 3; y++)
			{
				curTile = tilemap->GetTile(x, y);
				if (curTile == nullptr ||
					curTile->tileType != 0 ||
					curTile->zoneNumber != biggestZoneNumber ||
					curTile->doorIndex != -1)
					continue;

				breakCycle = false;
				for (int _x = x - 1; _x <= x + 1; _x++)
					if ((curTile = tilemap->GetTile(_x, y - 1)) == nullptr ||
						curTile->tileType == 0)
						breakCycle = true;
				if (!breakCycle && curTile != nullptr &&
					curTile->tileType == 1)
				{
					breakCycle = false;
					for (int _x = x - 2; _x <= x + 2 && !breakCycle; _x++)
						for (int _y = y - 5; _y <= y - 1 && !breakCycle; _y++)
						{
							curTile = tilemap->GetTile(_x, _y);
							if (curTile == nullptr ||
								(curTile->tileType == 0 && curTile->zoneNumber == biggestZoneNumber))
							{
								breakCycle = true;
								break;
							}
						}
					if (!breakCycle)
					{
						doorsCount++;
						int doorType = possibleDoorTypes[rand() % possibleDoorTypes.size()];
						for (int _x = x - 1; _x <= x + 1; _x++)
							for (int _y = y - 4; _y <= y - 2; _y++)
							{
								curTile = tilemap->GetTile(_x, _y);
								curTile->generationTileType = Tile::GenerationTileType::TrueFloor;
								curTile->tileType = 0;
							}

						curTile = tilemap->GetTile(x, y - 1);
						curTile->generationTileType = Tile::GenerationTileType::TrueFloor;
						curTile->tileType = 2;
						switch (curTile->doorColor = doorType)
						{
						case 0: curTile->requiredItemIdNameToUnlock = string("blue_key"); break;
						case 1: curTile->requiredItemIdNameToUnlock = string("green_key"); break;
						case 2: curTile->requiredItemIdNameToUnlock = string("red_key"); break;
						case 3: curTile->requiredItemIdNameToUnlock = string("purple_key"); break;
						case 4: curTile->requiredItemIdNameToUnlock = string("yellow_key"); break;
						}
						for (int i = 0; i < possibleDoorTypes.size(); i++)
							if (curTile->doorColor == possibleDoorTypes[i])
							{
								possibleDoorTypes.erase(possibleDoorTypes.begin() + i);
								break;
							}
						doorTiles.push_back(curTile);

						curTile = tilemap->GetTile(x, y - 2);
						int doorlessDistance = mapSizeX * mapSizeY;
						oldMarkedTiles = new List<Tile*>();
						newMarkedTiles = new List<Tile*>();
						neighborTile = curTile;

						oldMarkedTiles->Add(curTile);
						int markedCellsCount = 0;

						neighborTile->zoneNumber = biggestZoneNumber;
						neighborTile->doorIndex = doorType;
						neighborTile->canPlaceKey = false;
						if (!biggestZone->Contains(neighborTile))
							biggestZone->Add(neighborTile);
						while (doorlessDistance > 0)
						{
							doorlessDistance--;
							newMarkedTiles->RemoveList();
							//if (markedCellsCount - 2 == oldMarkedTiles->Count()) break;
							markedCellsCount = oldMarkedTiles->Count();
							for (int i = 0; i < markedCellsCount; i++)
								newMarkedTiles->Add((*oldMarkedTiles)[i]);
							markedCellsCount = (*newMarkedTiles).Count();
							oldMarkedTiles->RemoveList();
							for (int i = 0; i < markedCellsCount; i++)
							{
								curTile = (*newMarkedTiles)[i];
								neighborTile = tilemap->GetTile(curTile->x, curTile->y + 1);
								if (neighborTile != nullptr &&
									!oldMarkedTiles->Contains(neighborTile) &&
									neighborTile->tileType == 0 &&
									neighborTile->zoneNumber != biggestZoneNumber)
								{
									neighborTile->zoneNumber = biggestZoneNumber;
									neighborTile->doorIndex = doorType;
									neighborTile->canPlaceKey = false;
									if (!biggestZone->Contains(neighborTile))
										biggestZone->Add(neighborTile);
									oldMarkedTiles->Add(neighborTile);
									behindDoorTiles[doorType].push_back(neighborTile);
								}

								neighborTile = tilemap->GetTile(curTile->x, curTile->y - 1);
								if (neighborTile != nullptr &&
									!oldMarkedTiles->Contains(neighborTile) &&
									neighborTile->tileType == 0 &&
									neighborTile->zoneNumber != biggestZoneNumber)
								{
									neighborTile->zoneNumber = biggestZoneNumber;
									neighborTile->doorIndex = doorType;
									neighborTile->canPlaceKey = false;
									if (!biggestZone->Contains(neighborTile))
										biggestZone->Add(neighborTile);
									oldMarkedTiles->Add(neighborTile);
									behindDoorTiles[doorType].push_back(neighborTile);
								}

								neighborTile = tilemap->GetTile(curTile->x + 1, curTile->y);
								if (neighborTile != nullptr &&
									!oldMarkedTiles->Contains(neighborTile) &&
									neighborTile->tileType == 0 &&
									neighborTile->zoneNumber != biggestZoneNumber)
								{
									neighborTile->zoneNumber = biggestZoneNumber;
									neighborTile->doorIndex = doorType;
									neighborTile->canPlaceKey = false;
									if (!biggestZone->Contains(neighborTile))
										biggestZone->Add(neighborTile);
									oldMarkedTiles->Add(neighborTile);
									behindDoorTiles[doorType].push_back(neighborTile);
								}

								neighborTile = tilemap->GetTile(curTile->x - 1, curTile->y);
								if (neighborTile != nullptr &&
									!oldMarkedTiles->Contains(neighborTile) &&
									neighborTile->tileType == 0 &&
									neighborTile->zoneNumber != biggestZoneNumber)
								{
									neighborTile->zoneNumber = biggestZoneNumber;
									neighborTile->doorIndex = doorType;
									neighborTile->canPlaceKey = false;
									if (!biggestZone->Contains(neighborTile))
										biggestZone->Add(neighborTile);
									oldMarkedTiles->Add(neighborTile);
									behindDoorTiles[doorType].push_back(neighborTile);
								}
							}
						}

						delete oldMarkedTiles;
						delete newMarkedTiles;
						continue;
					}
				}

				breakCycle = false;
				for (int _x = x - 1; _x <= x + 1; _x++)
					if ((curTile = tilemap->GetTile(_x, y + 1)) == nullptr ||
						curTile->tileType == 0)
						breakCycle = true;
				if (!breakCycle && curTile != nullptr &&
					curTile->tileType == 1)
				{
					breakCycle = false;
					for (int _x = x - 2; _x <= x + 2 && !breakCycle; _x++)
						for (int _y = y + 1; _y <= y + 5 && !breakCycle; _y++)
						{
							curTile = tilemap->GetTile(_x, _y);
							if (curTile == nullptr ||
								(curTile->tileType == 0 && curTile->zoneNumber == biggestZoneNumber))
							{
								breakCycle = true;
								break;
							}
						}
					if (!breakCycle)
					{
						doorsCount++;
						int doorType = possibleDoorTypes[rand() % possibleDoorTypes.size()];
						for (int _x = x - 1; _x <= x + 1; _x++)
							for (int _y = y + 2; _y <= y + 4; _y++)
							{
								curTile = tilemap->GetTile(_x, _y);
								curTile->generationTileType = Tile::GenerationTileType::TrueFloor;
								curTile->tileType = 0;
							}

						curTile = tilemap->GetTile(x, y + 1);
						curTile->generationTileType = Tile::GenerationTileType::TrueFloor;
						curTile->tileType = 2;
						switch (curTile->doorColor = doorType)
						{
						case 0: curTile->requiredItemIdNameToUnlock = string("blue_key"); break;
						case 1: curTile->requiredItemIdNameToUnlock = string("green_key"); break;
						case 2: curTile->requiredItemIdNameToUnlock = string("red_key"); break;
						case 3: curTile->requiredItemIdNameToUnlock = string("purple_key"); break;
						case 4: curTile->requiredItemIdNameToUnlock = string("yellow_key"); break;
						}
						for (int i = 0; i < possibleDoorTypes.size(); i++)
							if (curTile->doorColor == possibleDoorTypes[i])
							{
								possibleDoorTypes.erase(possibleDoorTypes.begin() + i);
								break;
							}
						doorTiles.push_back(curTile);

						curTile = tilemap->GetTile(x, y + 2);
						int doorlessDistance = mapSizeX * mapSizeY;
						oldMarkedTiles = new List<Tile*>();
						newMarkedTiles = new List<Tile*>();
						neighborTile = curTile;

						oldMarkedTiles->Add(curTile);
						int markedCellsCount = 0;

						neighborTile->zoneNumber = biggestZoneNumber;
						neighborTile->doorIndex = doorType;
						neighborTile->canPlaceKey = false;
						if (!biggestZone->Contains(neighborTile))
							biggestZone->Add(neighborTile);
						while (doorlessDistance > 0)
						{
							doorlessDistance--;
							newMarkedTiles->RemoveList();
							//if (markedCellsCount - 2 == oldMarkedTiles->Count()) break;
							markedCellsCount = oldMarkedTiles->Count();
							for (int i = 0; i < markedCellsCount; i++)
								newMarkedTiles->Add((*oldMarkedTiles)[i]);
							markedCellsCount = (*newMarkedTiles).Count();
							oldMarkedTiles->RemoveList();
							for (int i = 0; i < markedCellsCount; i++)
							{
								curTile = (*newMarkedTiles)[i];
								neighborTile = tilemap->GetTile(curTile->x, curTile->y + 1);
								if (neighborTile != nullptr &&
									!oldMarkedTiles->Contains(neighborTile) &&
									neighborTile->tileType == 0 &&
									neighborTile->zoneNumber != biggestZoneNumber)
								{
									neighborTile->zoneNumber = biggestZoneNumber;
									neighborTile->doorIndex = doorType;
									neighborTile->canPlaceKey = false;
									if (!biggestZone->Contains(neighborTile))
										biggestZone->Add(neighborTile);
									oldMarkedTiles->Add(neighborTile);
									behindDoorTiles[doorType].push_back(neighborTile);
								}

								neighborTile = tilemap->GetTile(curTile->x, curTile->y - 1);
								if (neighborTile != nullptr &&
									!oldMarkedTiles->Contains(neighborTile) &&
									neighborTile->tileType == 0 &&
									neighborTile->zoneNumber != biggestZoneNumber)
								{
									neighborTile->zoneNumber = biggestZoneNumber;
									neighborTile->doorIndex = doorType;
									neighborTile->canPlaceKey = false;
									if (!biggestZone->Contains(neighborTile))
										biggestZone->Add(neighborTile);
									oldMarkedTiles->Add(neighborTile);
									behindDoorTiles[doorType].push_back(neighborTile);
								}

								neighborTile = tilemap->GetTile(curTile->x + 1, curTile->y);
								if (neighborTile != nullptr &&
									!oldMarkedTiles->Contains(neighborTile) &&
									neighborTile->tileType == 0 &&
									neighborTile->zoneNumber != biggestZoneNumber)
								{
									neighborTile->zoneNumber = biggestZoneNumber;
									neighborTile->doorIndex = doorType;
									neighborTile->canPlaceKey = false;
									if (!biggestZone->Contains(neighborTile))
										biggestZone->Add(neighborTile);
									oldMarkedTiles->Add(neighborTile);
									behindDoorTiles[doorType].push_back(neighborTile);
								}

								neighborTile = tilemap->GetTile(curTile->x - 1, curTile->y);
								if (neighborTile != nullptr &&
									!oldMarkedTiles->Contains(neighborTile) &&
									neighborTile->tileType == 0 &&
									neighborTile->zoneNumber != biggestZoneNumber)
								{
									neighborTile->zoneNumber = biggestZoneNumber;
									neighborTile->doorIndex = doorType;
									neighborTile->canPlaceKey = false;
									if (!biggestZone->Contains(neighborTile))
										biggestZone->Add(neighborTile);
									oldMarkedTiles->Add(neighborTile);
									behindDoorTiles[doorType].push_back(neighborTile);
								}
							}
						}

						delete oldMarkedTiles;
						delete newMarkedTiles;
						continue;
					}
				}
				//ceaseDoorGeneration = true;
			}

		explorableTilesCount = biggestZone->Count();
	}

	Tile* GetTileForPlayerSpawn()
	{
		return tileForPlayerToSpawn;
	}
	Tile* GetTileForCreatureSpawn()
	{
		if (tilesForCreaturesToSpawn.size() == 0)
			return nullptr;
		int index = rand() % tilesForCreaturesToSpawn.size();
		Tile* result = tilesForCreaturesToSpawn[index];
		tilesForCreaturesToSpawn.erase(tilesForCreaturesToSpawn.begin() + index);
		return result;
	}
	void PreUpdateMap(void)
	{
		/*mapPosition->x = (int)camera.x;
		mapPosition->y = (int)camera.y;
		mapPosition->w = (int)(Initializer.TEXTURE_RES * mapSizeX * zoom);
		mapPosition->h = (int)(Initializer.TEXTURE_RES * mapSizeY * zoom);*/
		mapScreen->x = -(int)camera.x;
		mapScreen->y = -(int)camera.y;

		/*SDL_RenderCopy(Initializer.renderer, mapTexture, nullptr, mapPosition);
		SDL_RenderCopy(Initializer.renderer, gridTexture, nullptr, mapPosition);*/
		SDL_RenderCopy(Initializer.renderer, mapTexture, mapScreen, mapPosition);
		//SDL_RenderCopy(Initializer.renderer, gridTexture, mapScreen, mapPosition);
	}
	void PostUpdateMap(void)
	{
		fogOfWarPosition->x = (int)camera.x;
		fogOfWarPosition->y = (int)camera.y;

		SDL_RenderCopy(Initializer.renderer, outOfSightLandsTexture, nullptr, fogOfWarPosition);
		//SDL_RenderCopy(Initializer.renderer, fogOfWarTexture, nullptr, fogOfWarPosition);
	}
};