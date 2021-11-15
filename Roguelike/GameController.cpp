#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define SDL_MAIN_HANDLED

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()
#define Map MapHandler::Instance()
#define Menu MenuHandler::Instance()
#define Overlay UIOverlayHandler::Instance()
#define Inventory InventoryHandler::Instance()
#define Items ItemsDatabase::Instance()
#define Sounds SoundController::Instance()

#include <iostream>
#include <io.h>
#include <fstream>
#include <random>
#include <windows.h>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#include "Time.h"
#include "MapController.h"
#include "MapHandler.h"
#include "MenuHandler.h"
#include "ItemsDatabase.h"
#include "Inventory.h"
#include "UIOverlayHandler.h"
#include "GameController.h"
#include "SoundController.h"
#include "VisualEffect.h"

#include "CreatureHero.h"
#include "CreatureGhost.h"
#include "CreatureEvilEye.h"
#include "CreatureElderEvilEye.h"
#include "CreatureBroodmother.h"
#include "CreatureSpiderling.h"
#include "CreatureBlackWidow.h"
#include "CreatureBlackSpiderling.h"
#include "BossTest.h"
#include "ObjectAltar.h"
using namespace std;

void GameController::SetPause(double pause)
{
	curPause = pause > 0 ? pause : 0;
}
double GameController::GetPause(void)
{
	return curPause;
}

bool GameController::Update(void)
{
	Time::SetNow(SDL_GetPerformanceCounter());
	// must be the first

	SDL_RenderClear(Initializer.renderer);

	switch (gameState)
	{
	case GameState::InMenu:
		EventHandle(&eventHandler);
		SDL_RenderSetScale(Initializer.renderer, 1, 1);
		Menu.RenderMenu();
		SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
		break;
	case GameState::InGame:
		EventHandle(&eventHandler);
		map->PreUpdateMap();

		/*if (!gameInPause &&
			GetPause() <= 0 &&
			currentCreature->isUnderPlayerControl &&
			currentCreature->GetMovementState() &&
			map->camera.GetCameraMovementState(map) &&
			currentCreature->actionsLeft <= 0 && !inSplashScreen)
			NextTurn();*/
		map->PostUpdateMap();

		{
			Tile* t;
			for (auto i = Map.GetVisibleTiles().begin(); i != Map.GetVisibleTiles().end(); i++)
			{
				t = map->tilemap->GetTile((*i)->x, (*i)->y);
				if (t != nullptr && t->tileItem != nullptr)
				{
					t->tileItem->Update(map);
					t->tileItem->Render(t->GetTileObject() != nullptr ? 128 : 255);
				}
			}
		}

		if (!gameInPause)
		{
			if (isSelectingTile && currentTileSelection != nullptr)
			{
				SDL_RenderSetScale(Initializer.renderer, 1, 1);
				SDL_Rect rect =
				{
					(int)(trunc((mouseX + map->zoom) / (Initializer.TEXTURE_RES * map->zoom) +
						(map->camera.x < 0) / 2.) * Initializer.TEXTURE_RES * map->zoom +
						(int)map->camera.x % Initializer.TEXTURE_RES * map->zoom),
					(int)(trunc((mouseY + map->zoom) / (Initializer.TEXTURE_RES * map->zoom) +
						(-map->camera.y == map->mapSizeY * Initializer.TEXTURE_RES - Initializer.SCREEN_HEIGHT / map->zoom) / 2.) * 
						Initializer.TEXTURE_RES * map->zoom +
						(int)map->camera.y % Initializer.TEXTURE_RES * map->zoom),
					Initializer.TEXTURE_RES * map->zoom,
					Initializer.TEXTURE_RES * map->zoom
				};
				if ((!currentTileSelection->isVisible && !Inventory.currentSlot->GetItem()->canUseInFogOfWar) ||
					Vector::GetDistance(
					{ 
						currentTileSelection->x, 
						currentTileSelection->y 
					}, mainHero->ToVector()) > Inventory.currentSlot->GetItem()->usingRange)
					SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileSelectionWrongSrc, &rect);
				else
					SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileSelectionSrc, &rect);
				SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
			}
			altar->Update();
		}

		// visual effects behind creatures
		if (!gameInPause)
		{
			for (int i = 0; i < visualEffects.size();)
			{
				if (!visualEffects[i]->playBehindCreature)
				{
					i++;
					continue;
				}
				if (visualEffects[i]->PlayAnimationOnce())
				{
					delete visualEffects[i];
					visualEffects.erase(visualEffects.begin() + i);
					continue;
				}

				visualEffects[i]->CalculateAnimationCoordinates();
				visualEffects[i]->textureRect =
				{
					(int)(Controller.map->camera.x - visualEffects[i]->animationCoordX),
					(int)(Controller.map->camera.y - visualEffects[i]->animationCoordY - Initializer.TEXTURE_RES / 4),
					visualEffects[i]->textureSrc.w,
					visualEffects[i]->textureSrc.h
				};
				SDL_RenderCopy(
					Initializer.renderer,
					visualEffects[i]->effectTexture,
					&visualEffects[i]->textureSrc,
					&visualEffects[i]->textureRect);
				i++;
			}
		}

		if (altar->y <= mainHero->y)
			altar->DrawTexture();
		for (int i = 0; i < creaturesQueue.size()/*creaturesCount*/; i++)
			if (!creaturesQueue[creaturesIndexes[i]]->isAlive)
				creaturesQueue[creaturesIndexes[i]]->DrawTexture();
		for (int i = 0; i < creaturesQueue.size()/*creaturesCount*/; i++)
		{
			if (creaturesQueue[creaturesIndexes[i]]->isAlive)
				creaturesQueue[creaturesIndexes[i]]->DrawTexture();
			if (!gameInPause)
				creaturesQueue[i]->Update();
		}
		if (altar->y > mainHero->y)
			altar->DrawTexture();

		VisualEffect* effect;
		if (!gameInPause)
		{
			for (int i = 0; i < creaturesQueue.size()/*creaturesCount*/; i++)
			{
				for (int j = 0; j < creaturesQueue[creaturesIndexes[i]]->visualEffects.size(); j++)
				{
					effect = creaturesQueue[creaturesIndexes[i]]->visualEffects[j];
					effect->PlayAnimation();
					effect->CalculateAnimationCoordinates(
						creaturesQueue[creaturesIndexes[i]]->visualX,
						creaturesQueue[creaturesIndexes[i]]->visualY);
					effect->textureRect =
					{
						(int)(Controller.map->camera.x - effect->animationCoordX),
						(int)(Controller.map->camera.y - effect->animationCoordY - Initializer.TEXTURE_RES / 4),
						effect->textureSrc.w,
						effect->textureSrc.h
					};
					SDL_RenderCopy(
						Initializer.renderer,
						effect->effectTexture,
						&effect->textureSrc,
						&effect->textureRect);
					i++;
				}
			}

			// visual effects in front of creatures
			for (int i = 0; i < visualEffects.size();)
			{
				if (visualEffects[i]->playBehindCreature)
				{
					i++;
					continue;
				}
				visualEffects[i]->CalculateAnimationCoordinates();
				if (visualEffects[i]->PlayAnimationOnce())
				{
					delete visualEffects[i];
					visualEffects.erase(visualEffects.begin() + i);
					continue;
				}

				// camera shake
				if (visualEffects[i]->effectName == "astral_greatsword")
				{
					if (visualEffects[i]->GetCurrentFrameIndex() == 3 || visualEffects[i]->GetCurrentFrameIndex() == 4)
					{
						map->camera.cameraMovementSpeed = 100;
						if (map->camera.x != 0 && -map->camera.x != map->mapSizeX * Initializer.TEXTURE_RES - Initializer.SCREEN_WIDTH / map->zoom &&
							map->camera.y != 0 && -map->camera.y != map->mapSizeY * Initializer.TEXTURE_RES - Initializer.SCREEN_HEIGHT / map->zoom)
							map->camera.CameraTeleport(map,
								-map->camera.target->x - 0.35 + ((double)rand() / RAND_MAX) * 0.7,
								-map->camera.target->y - 0.35 + ((double)rand() / RAND_MAX) * 0.7);
					}
					if (visualEffects[i]->GetCurrentFrameIndex() > 4)
						map->camera.cameraMovementSpeed = 350;
				}

				if (visualEffects[i]->effectName == "starlight_eruption")
				{
					ScreenFade(
						visualEffects[i]->GetCurrentFrameIndex() < 11,
						180,
						255 * Time::GetDeltaTime(), &fadeState);
					visualEffects[i]->textureRect =
					{
						(int)(Controller.map->camera.x - visualEffects[i]->animationCoordX),
						(int)(Controller.map->camera.y - visualEffects[i]->animationCoordY - Initializer.TEXTURE_RES / 8),
						visualEffects[i]->textureSrc.w,
						visualEffects[i]->textureSrc.h
					};
				}
				else
				{
					visualEffects[i]->textureRect =
					{
						(int)(Controller.map->camera.x - visualEffects[i]->animationCoordX),
						(int)(Controller.map->camera.y - visualEffects[i]->animationCoordY - Initializer.TEXTURE_RES / 4),
						visualEffects[i]->textureSrc.w,
						visualEffects[i]->textureSrc.h
					};
				}
				
				SDL_RenderCopy(
					Initializer.renderer, 
					visualEffects[i]->effectTexture, 
					&visualEffects[i]->textureSrc, 
					&visualEffects[i]->textureRect);
				i++;
			}
		}

		for (int i = 0; i < creaturesQueue.size()/*creaturesCount*/; i++)
			if (creaturesQueue[creaturesIndexes[i]]->isAlive)
				creaturesQueue[creaturesIndexes[i]]->DrawHealthbar();

		for (int i = 0; i < creaturesQueue.size()/*creaturesCount*/; i++)
			if (dynamic_cast<Boss*>(creaturesQueue[creaturesIndexes[i]]) != nullptr)
				creaturesQueue[creaturesIndexes[i]]->DrawHealthbar();

		if (!inSplashScreen)
		{
			if (!gameInPause)
			{
				Inventory.Render();
				if (sessionDay <= 0 && Inventory.inventoryIsShown)
					Inventory.RenderHint();

				SDL_RenderSetScale(Initializer.renderer, 3.5, 3.5);
				SDL_RenderCopy(Initializer.renderer, playerHealthbarTexture, playerHealthbarEmptySrc, playerHealthbarPosition);
				SDL_RenderCopy(Initializer.renderer, playerHealthbarTexture, playerHealthbarSrc, playerHealthbarFullPosition);
				SDL_RenderCopy(Initializer.renderer, playerHealthbarTexture, playerHealthbarForegroundSrc, playerHealthbarPosition);

				SDL_RenderCopy(Initializer.renderer, playerHealthbarTexture, playerHealthbarEmptySrc, playerEnergybarPosition);
				SDL_RenderCopy(Initializer.renderer, playerHealthbarTexture, playerEnergybarSrc, playerEnergybarFullPosition);
				SDL_RenderCopy(Initializer.renderer, playerHealthbarTexture, playerHealthbarForegroundSrc, playerEnergybarPosition);
				SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
			}
			else
			/*if (gameInPause && /*MapHandler::Instance().minimapIsShown ||
				(Menu.currentState != nullptr &&
					Menu.currentState->GetStateName() == "ingame_menu"))*/
			{
				SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, 150);
				SDL_RenderFillRect(Initializer.renderer, nullptr);
			}
			MapHandler::Instance().ShowMinimap(map, mainHero);
		}
		
		SDL_RenderSetScale(Initializer.renderer, 1, 1);
		if (!gameInPause && !inSplashScreen)
			Overlay.Render();
		Menu.RenderMenu();
		SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);

		if (!gameInPause && map->camera.CameraFollow(map) && currentCreature->GetMovementState())
			if (curPause > 0)
				curPause -= Time::GetDeltaTime();
		break;
	}

	if (splashScreen != nullptr && (gameState == GameState::InMenu || (gameInPause || GetPause() <= 0)))
		if (splashScreen->Play())
		{
			delete splashScreen;
			splashScreen = nullptr;
		}

	SDL_RenderPresent(Initializer.renderer);
	Sounds.RecalculateSounds();

	if (SDL_GetTicks() - Time::GetTimeSinceStart() < 15)
		SDL_Delay(15);

	// must be the last
	Time::Calculate();

	return gameIsRunning;
}

void GameController::NextTurn(void)
{
	currentCreature->EndTurn();
	//if (!currentCreature->EndTurn()) return;
	do
	{
		currentCreatureIndex++;
		if (currentCreatureIndex > creaturesQueue.size()/*creaturesCount*/ - 1)
			currentCreatureIndex = 0;
		currentCreature = creaturesQueue[currentCreatureIndex];
	}
	while (!currentCreature->isAlive);
	currentCreature->StartTurn();
	RecalculateOverlay();
}
void GameController::NextDay(void)
{
	gameInPause = true;
	//gameState = GameController::GameState::InMenu;

	sessionDay++;
	splashScreenText->texts[0].text = L"День " + to_wstring(sessionDay + 1);
	splashScreenText->texts[0].RebuildText();
	splashScreenText->Initialize(
		Initializer.SCREEN_WIDTH / 2,
		Initializer.SCREEN_HEIGHT / 2 - splashScreenText->GetHeight() / 2);
	if (splashScreen != nullptr)
		delete splashScreen;
	splashScreen = new GameController::SplashScreen(255, 1, 1, 1, 1, 1, splashScreenText);
	splashScreen->onFullTextIn =
		[&]()
	{
		FILE* curSave;
		fopen_s(&curSave, ("saves/" + saveFileName + ".dat").c_str(), "rb+");
		if (!curSave)
		{
			cout << "Save file is corrupted or does not exist." << endl;
			return;
		}
		int size = sessionName.size();
		fseek(curSave, size + sizeof(long long) + 4 * sizeof(int), 0);
		/*fwrite(&size, sizeof(int), 1, curSave);
		for (int i = 0; i < size; i++)
			fwrite(&sessionName.c_str()[i], sizeof(char), 1, curSave);
		fwrite(&sessionSeed, sizeof(long long), 1, curSave);
		fwrite(&sessionDay, sizeof(int), 1, curSave);
		fwrite(&sessionExploredTilesCount, sizeof(int), 1, curSave);
		fwrite(&sessionMaxPossibleExploredTilesCount, sizeof(int), 1, curSave);*/

		WriteInventoryInFile(curSave);
		fclose(curSave);

		if (altar != nullptr)
		{
			delete altar;
			altar = nullptr;
		}
		//if (creaturesQueue != nullptr)
		{
			/*for (int i = 0; i < map->mapSizeY; i++)
				for (auto j = creatures[i].begin(); j != creatures[i].end(); j++)
					delete *j;*/
			for (int i = 0; i < creaturesQueue.size()/*creaturesCount*/; i++)
				if (creaturesQueue[i] != nullptr)
				{
					delete creaturesQueue[i];
					creaturesQueue[i] = nullptr;
				}

			creaturesQueue.clear();
			creaturesIndexes.clear();
			//delete /*creaturesQueue,*/ creaturesIndexes;
			//creaturesQueue = nullptr;
			//creaturesIndexes = nullptr;
		}

		delete map;
		map = new MapController();
		//Controller.InitializeOverlayAndInventory();
		StartGame(true, 0);

		// write in file
		fopen_s(&curSave, ("saves/" + saveFileName + ".dat").c_str(), "rb+");
		if (!curSave)
		{
			cout << "Save file is corrupted or does not exist." << endl;
			return;
		}
		fseek(curSave, 0, 0);
		size = sessionName.size();
		fwrite(&size, sizeof(int), 1, curSave);
		for (int i = 0; i < size; i++)
			fwrite(&sessionName.c_str()[i], sizeof(char), 1, curSave);
		fwrite(&sessionSeed, sizeof(long long), 1, curSave);
		fwrite(&sessionDay, sizeof(int), 1, curSave);
		fwrite(&sessionExploredTilesCount, sizeof(int), 1, curSave);
		fwrite(&sessionMaxPossibleExploredTilesCount, sizeof(int), 1, curSave);
		fclose(curSave);

		fopen_s(&curSave, ("saves/" + saveFileName + ".dat").c_str(), "rb+");
		if (!curSave)
		{
			cout << "Save file is corrupted or does not exist." << endl;
			return;
		}
		fseek(curSave, size + sizeof(long long) + 4 * sizeof(int), 0);
		Controller.ReadInventoryFromFile(curSave);

		fclose(curSave);

		gameState = GameController::GameState::InGame;
	};
	splashScreen->onFullTextOut =
		[&]()
	{
		Menu.SetState(nullptr);
		gameInPause = false;
	};
}
void GameController::RecalculateOverlay(void)
{
	Overlay.ClearOverlay();
	if (shardsCount >= shardsRequired)
		altar->canInteractWith = true;
	if (sessionDay <= 0)
	{
		if (shardsCount < shardsRequired)
			Overlay.AddLabel(L"Уничтожайте врагов, чтобы получить осколки", 35, { 255, 255, 0 }, Overlay.topRightCorner);
		else
			Overlay.AddLabel(L"Используйте Колодец для перехода на новый день", 35, { 255, 255, 0 }, Overlay.topRightCorner);
	}
	Overlay.AddLabel(L"Осколков: " + to_wstring(shardsCount) + L" / " + to_wstring(shardsRequired), 50, { 255, 255, 255 }, Overlay.topRightCorner);
	Overlay.AddGap(Overlay.topRightCorner);
	Overlay.AddLabel(L"Урон: " + to_wstring(mainHero->damage), 50, { 255, 255, 255 }, Overlay.topRightCorner);
	Overlay.AddLabel(to_wstring(mainHero->curHealth) + L" / " + to_wstring(mainHero->maxHealth), 50, { 255, 255, 255 }, Overlay.topLeftCorner);
	Overlay.AddGap(Overlay.topLeftCorner);
	Overlay.AddGap(Overlay.topLeftCorner);
	Overlay.AddLabel(to_wstring(mainHero->actionsLeft) + L" / " + to_wstring(mainHero->maxActionsCount), 50, {255, 255, 255}, Overlay.topLeftCorner);
	if (currentCreature->isUnderPlayerControl)
	{
		Tile* t = map->tilemap->GetTileInFront(mainHero);
		TileObject* obj = t->GetTileObject();
		if (t->tileType == 0)
		{
			if (obj == nullptr)
				Overlay.AddImageWithLabel(
					{ 300, (int)mainHero->facingDirection * 16, 16, 16 },
					L"идти", 50, { 255, 255, 255 }, Overlay.bottomRightCorner);
			else if (dynamic_cast<Creature*>(obj) != nullptr)
				Overlay.AddImageWithLabel(
					{ 300, 4 * 16, 16, 16 },
					L"атаковать", 50, { 255, 255, 255 }, Overlay.bottomRightCorner);
			else if (obj->canInteractWith)
				Overlay.AddImageWithLabel(
					{ 300, 5 * 16, 16, 16 },
					L"взаимодействовать", 50, { 255, 255, 255 }, Overlay.bottomRightCorner);
		}
		else if (t->doorColor != -1 && Inventory.FindSlotWithItem(t->requiredItemIdNameToUnlock) != nullptr)
			Overlay.AddImageWithLabel(
				{ 300, 5 * 16, 16, 16 },
				L"взаимодействовать", 50, { 255, 255, 255 }, Overlay.bottomRightCorner);
		Overlay.AddImageWithLabel(
			{ 300, 6 * 16, 16, 16 },
			L"закончить ход", 50, { 255, 255, 255 }, Overlay.bottomRightCorner);
	}
	else
		Overlay.AddImageWithLabel(
			{ 300, 7 * 16, 16, 16 },
			L"враги ходят...", 50, { 255, 255, 255 }, Overlay.bottomRightCorner);
	if (!Inventory.inventoryIsShown)
		Overlay.AddImageWithLabel(
			{ 316, 4 * 16, 16, 16 },
			L"открыть инвентарь", 50, { 255, 255, 255 }, Overlay.bottomRightCorner);
	if (!Map.minimapIsShown)
		Overlay.AddImageWithLabel(
			{ 316, 5 * 16, 16, 16 },
			L"открыть карту", 50, { 255, 255, 255 }, Overlay.bottomRightCorner);
	Overlay.Calculate();
}
void GameController::RecalculateRenderLayers(void)
{
	int min, buf;
	for (int i = 0; i < creaturesQueue.size(); i++)
	{
		min = i;
		for (int j = i + 1; j < creaturesQueue.size(); j++)
			min = (creaturesQueue[creaturesIndexes[j]]->y < creaturesQueue[creaturesIndexes[min]]->y) ? j : min;
		if (i != min)
		{
			buf = creaturesIndexes[i];
			creaturesIndexes[i] = creaturesIndexes[min];
			creaturesIndexes[min] = buf;
		}
	}
}
void GameController::BeginTileSelection()
{
	isSelectingTile = true;
	Inventory.inventoryIsShown = false;
	currentTileSelection = nullptr;
}
void GameController::EndTileSelection(Tile* selectedTile)
{
	isSelectingTile = false;
	currentTileSelection = nullptr;
	Inventory.inventoryIsShown = true;
	Inventory.UseItem(Inventory.currentSlot, selectedTile);
	Inventory.currentSlot = nullptr;
	if (!gameInPause && Inventory.inventoryIsShown)
		Inventory.CheckMousePosition(mouseX, mouseY);
}
void GameController::EventHandle(SDL_Event *_event)
{
	while (SDL_PollEvent(_event) != NULL)
	{
		if (inSplashScreen && _event->type != SDL_QUIT)
			break;
		switch (gameState)
		{
		case GameState::InMenu:
			switch (_event->type)
			{
			case SDL_QUIT:
				gameIsRunning = false;
				quitGame = true;
				break;

			case SDL_MOUSEMOTION:
				if (Menu.currentState == nullptr)
					break;
				Menu.CheckMousePosition(_event->motion.x, _event->motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (Menu.currentState == nullptr)
					break;
				Menu.CheckMouseButtonDown(_event->button.button);
				break;

			case SDL_MOUSEBUTTONUP:
				if (Menu.currentState == nullptr)
					break;
				Menu.CheckMouseButtonUp(_event->button.button);
				break;

			case SDL_KEYDOWN:
				if (Menu.currentState == nullptr)
					break;
				if (_event->key.keysym.sym == SDLK_BACKSPACE)
					if (Menu.selectedItem != nullptr &&
						dynamic_cast<MenuItemTextbox*>(Menu.selectedItem))
						if (_event->key.keysym.sym == SDLK_BACKSPACE)
							((MenuItemTextbox*)Menu.selectedItem)->RemoveCharacter();
				break;

			case SDL_TEXTINPUT:
				if (Menu.currentState == nullptr)
					break;
				if (_event->text.text[0] >= 32 && _event->text.text[0] < 128 && !(SDL_GetModState() & KMOD_CTRL &&
					(_event->text.text[0] == 'c' ||
						_event->text.text[0] == 'C' ||
						_event->text.text[0] == 'v' ||
						_event->text.text[0] == 'V')))
				{
					if (Menu.selectedItem != nullptr &&
						dynamic_cast<MenuItemTextbox*>(Menu.selectedItem))
						((MenuItemTextbox*)Menu.selectedItem)->EnterCharacter((wchar_t)_event->text.text[0]);
				}
				break;
			}
			break;
		case GameState::InGame:
			switch (_event->type)
			{
			case SDL_QUIT:
				gameIsRunning = false;
				quitGame = true;
				break;

			case SDL_MOUSEMOTION:
			{
				mouseX = _event->motion.x;
				mouseY = _event->motion.y;
				if (isSelectingTile)
				{
					// camera coordinates to world coordinates
					int _x = (int)trunc((mouseX - map->camera.x * map->zoom) / (Initializer.TEXTURE_RES * map->zoom));
					int _y = (int)trunc((mouseY - map->camera.y * map->zoom) / (Initializer.TEXTURE_RES * map->zoom));
					currentTileSelection = map->tilemap->GetTile(_x, _y);
				}

				if (!gameInPause && Inventory.inventoryIsShown)
					Inventory.CheckMousePosition(mouseX, mouseY);
				if (Menu.currentState == nullptr)
					break;
				Menu.CheckMousePosition(mouseX, mouseY);
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
				if (!gameInPause && Inventory.inventoryIsShown && !isSelectingTile && currentCreature == mainHero)
					Inventory.CheckMouseButtonDown(_event->button.button);
				if (Menu.currentState == nullptr)
					break;
				Menu.CheckMouseButtonDown(_event->button.button);
				break;

			case SDL_MOUSEBUTTONUP:
				if (isSelectingTile && currentTileSelection != nullptr)
				{
					if (_event->button.button != SDL_BUTTON_LEFT ||
						(!currentTileSelection->isVisible && !Inventory.currentSlot->GetItem()->canUseInFogOfWar) ||
							Vector::GetDistance(
								{
									currentTileSelection->x,
									currentTileSelection->y
								}, mainHero->ToVector()) > Inventory.currentSlot->GetItem()->usingRange)
						EndTileSelection(nullptr);
					else
						EndTileSelection(currentTileSelection);
				}
				if (!gameInPause && Inventory.inventoryIsShown && !isSelectingTile && currentCreature == mainHero)
					Inventory.CheckMouseButtonUp(_event->button.button);
				if (Menu.currentState == nullptr)
					break;
				Menu.CheckMouseButtonUp(_event->button.button);
				break;

			case SDL_KEYDOWN:
				Inventory.dragFrom = nullptr;
				Inventory.isDraggingItem = false;

				if (_event->key.keysym.sym == SDLK_i)
				{
					if (_event->key.repeat == 0)
						Inventory.inventoryIsShown = !Inventory.inventoryIsShown;
				}
				else if (isSelectingTile) 
					EndTileSelection(nullptr);

				if (_event->key.keysym.sym == SDLK_ESCAPE && _event->key.repeat == 0)
				{
					//gameIsRunning = false;
					if (Menu.currentState != nullptr &&
						Menu.currentState->GetStateName() == "ingame_menu")
					{
						Menu.SetState(nullptr);
						gameInPause = false;
					}
					else
					{
						MapHandler::Instance().minimapIsShown = false;
						Menu.SetState(Menu.GetStateByName("ingame_menu"));
						gameInPause = true;
					}
					
					break;
				}
				if (_event->key.keysym.sym == SDLK_m && _event->key.repeat == 0)
				{
					if (gameInPause)
					{
						if (Menu.currentState != nullptr &&
							Menu.currentState->GetStateName() == "ingame_menu")
						{
							Menu.SetState(nullptr);
							Map.minimapIsShown = true;
						}
						else
						{
							Map.minimapIsShown = false;
							gameInPause = false;
						}
					}
					else
					{
						MapHandler::Instance().minimapIsShown = true;
						gameInPause = true;
					}
				}
				if (_event->key.keysym.sym == SDLK_UP && _event->key.repeat == 0)
				{
					if (gameInPause)
						break;
					if (currentCreature->isUnderPlayerControl && currentCreature->GetMovementState() && map->camera.GetCameraMovementState(map))
					{
						Tile *tile = map->tilemap->GetTile(currentCreature->x, currentCreature->y - 1);
						if (currentCreature->facingDirection == currentCreature->GetRequiredFacingDirection(tile) && currentCreature->actionsLeft > 0)
						{
							if (tile->doorColor == -1 && currentCreature->MoveTo(tile))
							{
								if (tile->tileItem != nullptr && Inventory.HasEmptySlot())
									tile->PickUpTileItem(currentCreature, tile->tileItem->connectedItemIdName);
								Map.RecalculateFogOfWar(map,
									map->tilemap->GetTile(currentCreature->x, currentCreature->y),
									currentCreature->viewDistance);
								Map.RecalculateVisibleLands(map,
									map->tilemap->GetTile(currentCreature->x, currentCreature->y),
									currentCreature->sightDistance);
								if (!godMode)
									currentCreature->actionsLeft--;
							}
						}
						else currentCreature->SetFacingDirection(tile);
					}
					else if (map->camera.freeCamera)
					{
						map->camera.CameraTeleport(map, map->camera.tileX, map->camera.tileY);
						map->camera.tileY++;
					}
					Controller.RecalculateRenderLayers();
				}
				else if (_event->key.keysym.sym == SDLK_DOWN && _event->key.repeat == 0)
				{
					if (gameInPause)
						break;
					if (currentCreature->isUnderPlayerControl && currentCreature->GetMovementState() && map->camera.GetCameraMovementState(map))
					{
						Tile *tile = map->tilemap->GetTile(currentCreature->x, currentCreature->y + 1);
						if (currentCreature->facingDirection == currentCreature->GetRequiredFacingDirection(tile) && currentCreature->actionsLeft > 0)
						{
							if (tile->doorColor == -1 && currentCreature->MoveTo(tile))
							{
								if (tile->tileItem != nullptr && Inventory.HasEmptySlot())
									tile->PickUpTileItem(currentCreature, tile->tileItem->connectedItemIdName);
								Map.RecalculateFogOfWar(map,
									map->tilemap->GetTile(currentCreature->x, currentCreature->y),
									currentCreature->viewDistance);
								Map.RecalculateVisibleLands(map,
									map->tilemap->GetTile(currentCreature->x, currentCreature->y),
									currentCreature->sightDistance);
								if (!godMode)
									currentCreature->actionsLeft--;
							}
						}
						else currentCreature->SetFacingDirection(tile);
					}
					else if (map->camera.freeCamera)
					{
						map->camera.CameraTeleport(map, map->camera.tileX, map->camera.tileY);
						map->camera.tileY--;
					}
					Controller.RecalculateRenderLayers();
				}
				else if (_event->key.keysym.sym == SDLK_RIGHT && _event->key.repeat == 0)
				{
					if (gameInPause)
						break;
					if (currentCreature->isUnderPlayerControl && currentCreature->GetMovementState() && map->camera.GetCameraMovementState(map))
					{
						Tile *tile = map->tilemap->GetTile(currentCreature->x + 1, currentCreature->y);
						if (currentCreature->facingDirection == currentCreature->GetRequiredFacingDirection(tile) && currentCreature->actionsLeft > 0)
						{
							if (tile->doorColor == -1 && currentCreature->MoveTo(tile))
							{
								if (tile->tileItem != nullptr && Inventory.HasEmptySlot())
									tile->PickUpTileItem(currentCreature, tile->tileItem->connectedItemIdName);
								Map.RecalculateFogOfWar(map,
									map->tilemap->GetTile(currentCreature->x, currentCreature->y),
									currentCreature->viewDistance);
								Map.RecalculateVisibleLands(map,
									map->tilemap->GetTile(currentCreature->x, currentCreature->y),
									currentCreature->sightDistance);
								if (!godMode)
									currentCreature->actionsLeft--;
							}
						}
						else currentCreature->SetFacingDirection(tile);
					}
					else if (map->camera.freeCamera)
					{
						map->camera.CameraTeleport(map, map->camera.tileX, map->camera.tileY);
						map->camera.tileX--;
					}
				}
				else if (_event->key.keysym.sym == SDLK_LEFT && _event->key.repeat == 0)
				{
					if (gameInPause)
						break;
					if (currentCreature->isUnderPlayerControl && currentCreature->GetMovementState() && map->camera.GetCameraMovementState(map))
					{
						Tile *tile = map->tilemap->GetTile(currentCreature->x - 1, currentCreature->y);
						if (currentCreature->facingDirection == currentCreature->GetRequiredFacingDirection(tile) && currentCreature->actionsLeft > 0)
						{
							if (tile->doorColor == -1 && currentCreature->MoveTo(tile))
							{
								if (tile->tileItem != nullptr && Inventory.HasEmptySlot())
									tile->PickUpTileItem(currentCreature, tile->tileItem->connectedItemIdName);
								Map.RecalculateFogOfWar(map,
									map->tilemap->GetTile(currentCreature->x, currentCreature->y),
									currentCreature->viewDistance);
								Map.RecalculateVisibleLands(map,
									map->tilemap->GetTile(currentCreature->x, currentCreature->y),
									currentCreature->sightDistance);
								if (!godMode)
									currentCreature->actionsLeft--;
							}
						}
						else currentCreature->SetFacingDirection(tile);
					}
					else if (map->camera.freeCamera)
					{
						map->camera.CameraTeleport(map, map->camera.tileX, map->camera.tileY);
						map->camera.tileX++;
					}
				}
				if (_event->key.keysym.sym == SDLK_LCTRL && _event->key.repeat == 0)
				{
					if (gameInPause)
						break;
					if (currentCreature->isUnderPlayerControl && currentCreature->GetMovementState() && map->camera.GetCameraMovementState(map))
					{
						NextTurn();
					}
				}
				if (_event->key.keysym.sym == SDLK_LALT && _event->key.repeat == 0)
				{
					if (gameInPause)
						break;
					map->camera.freeCamera = !map->camera.freeCamera;
				}
				if (_event->key.keysym.sym == SDLK_LSHIFT && _event->key.repeat == 0)
				{
					//Sounds.PlaySoundOnce("sounds/abc.wav");
					/*if (currentCreature->isUnderPlayerControl && currentCreature->GetMovementState() && map->camera.GetCameraMovementState(map))
						Map.RecalculateFogOfWar(map,
							map->tilemap->GetTile(currentCreature->x, currentCreature->y),
							currentCreature->viewDistance * 10);*/
				}
				if (_event->key.keysym.sym == SDLK_EQUALS /*&& MapHandler::Instance().minimapMaxSize < 21*/ /*&& _event->key.repeat == 0*/)
				{
					if (map->camera.freeCamera)
						map->zoom += 0.05;
					/*if (MapHandler::Instance().minimapIsShown)
					{
						MapHandler::Instance().minimapScale++;
						MapHandler::Instance().minimapMaxSize += 2;
						MapHandler::Instance().RecalculateMinimap(map);
					}*/
				}
				if (_event->key.keysym.sym == SDLK_MINUS /*&& MapHandler::Instance().minimapMaxSize > 7*/ /*&& _event->key.repeat == 0*/)
				{
					if (map->camera.freeCamera)
						map->zoom -= 0.05;
					/*if (MapHandler::Instance().minimapIsShown)
					{
						MapHandler::Instance().minimapScale--;
						MapHandler::Instance().minimapMaxSize -= 2;
						MapHandler::Instance().RecalculateMinimap(map);
					}*/
				}
				if (_event->key.keysym.sym == SDLK_TAB && _event->key.repeat == 0)
				{
					//Creature::DealDamage(100, mainHero, nullptr);
					NextDay();
				}
				if (_event->key.keysym.sym == SDLK_a && _event->key.repeat == 0)
				{
					if (gameInPause)
						break;
					if (currentCreature->isUnderPlayerControl && currentCreature->GetMovementState() && map->camera.GetCameraMovementState(map))
					{
						Item* curItem = Inventory.equipmentSlot->GetItem();
						if (curItem != nullptr && curItem->itemIdName == "astral_greatsword")
						{
							if (currentCreature->actionsLeft >= 2 || godMode)
							{
								currentCreature->OnAttack();
								if (!godMode)
									currentCreature->actionsLeft -= 2;
								Creature* inFront = dynamic_cast<Creature*>(map->tilemap->GetTileObjectInFront(currentCreature));
								if (currentCreature == mainHero && Inventory.equipmentSlot->GetItem() != nullptr)
									Inventory.equipmentSlot->GetItem()->OnAttackWith(mainHero, inFront);
								if (inFront != nullptr)
									Creature::DealDamage(currentCreature->damage, inFront, currentCreature);
							}
						}
						else
						{
							if (currentCreature->actionsLeft > 0 || godMode)
							{
								currentCreature->OnAttack();
								if (!godMode)
									currentCreature->actionsLeft--;
								Creature* inFront = dynamic_cast<Creature*>(map->tilemap->GetTileObjectInFront(currentCreature));
								if (currentCreature == mainHero && Inventory.equipmentSlot->GetItem() != nullptr)
									Inventory.equipmentSlot->GetItem()->OnAttackWith(mainHero, inFront);
								if (inFront != nullptr)
									Creature::DealDamage(currentCreature->damage, inFront, currentCreature);
							}
						}
					}
				}
				if (_event->key.keysym.sym == SDLK_e && _event->key.repeat == 0)
				{
					if (gameInPause)
						break;
					if (currentCreature->isUnderPlayerControl && currentCreature->actionsLeft > 0 && currentCreature->GetMovementState() && map->camera.GetCameraMovementState(map))
					{
						Tile *tile = map->tilemap->GetTileInFront(currentCreature);
						if (tile->GetTileObject() != nullptr)
							tile->GetTileObject()->OnInteractWith(currentCreature);
						if (tile->doorColor != -1 && Inventory.FindItem(tile->requiredItemIdNameToUnlock))
						{
							SDL_SetRenderTarget(Initializer.renderer, map->mapTexture);
							if (map->zoom < 1)
								SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
							SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, 0);
							SDL_SetRenderDrawBlendMode(Initializer.renderer, SDL_BLENDMODE_NONE);
							SDL_SetTextureBlendMode(map->mapTexture, SDL_BLENDMODE_BLEND);

							SDL_Rect tileRect =
							{
								tile->x * Initializer.TEXTURE_RES,
								tile->y * Initializer.TEXTURE_RES,
								Initializer.TEXTURE_RES,
								Initializer.TEXTURE_RES
							};
							SDL_RenderFillRect(Initializer.renderer, &tileRect);
							SDL_Rect tileScr =
							{
								0,
								0,
								Initializer.TEXTURE_RES,
								Initializer.TEXTURE_RES
							};
							SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
							tileScr =
							{
								5 * Initializer.TEXTURE_RES,
								2 * Initializer.TEXTURE_RES,
								Initializer.TEXTURE_RES,
								Initializer.TEXTURE_RES
							};
							SDL_RenderCopy(Initializer.renderer, Initializer.tilesetFloor, &tileScr, &tileRect);
							tile->doorColor = -1;

							delete Inventory.FindSlotWithItem(tile->requiredItemIdNameToUnlock)->PullItem();

							SDL_SetRenderTarget(Initializer.renderer, nullptr);
							if (map->zoom < 1)
								SDL_RenderSetScale(Initializer.renderer, (float)map->zoom, (float)map->zoom);
							/*if (tile->tileItem != nullptr)
								tile->PickUpTileItem(currentCreature, tile->tileItem->connectedItemIdName);
							MapHandler::Instance().RecalculateFogOfWar(map,
								map->tilemap->GetTile(currentCreature->x, currentCreature->y),
								currentCreature->viewDistance);
							MapHandler::Instance().RecalculateVisibleLands(map,
								map->tilemap->GetTile(currentCreature->x, currentCreature->y),
								currentCreature->sightDistance);
							currentCreature->actionsLeft--;*/
						}
					}
				}
				if (_event->key.keysym.sym == SDLK_QUOTE && _event->key.repeat == 0)
				{
					char c;
					string command;
					while ((c = getchar()) != ' ' && c != '\n')
						command += c;
					if (c != '\n')
					{
						if (command == "give")
						{
							command.clear();
							while ((c = getchar()) != '\n')
								command += c;
							Item* i;
							if ((i = Items.GetItemByName(command)) != nullptr &&
								Inventory.HasEmptySlot())
								Inventory.AddItemToInventory(new Item(*i));
						}
						else if (command == "set_health")
						{
							command.clear();
							while ((c = getchar()) != '\n')
								command += c;
							int amount;
							try
							{
								amount = stoi(command.c_str());
							}
							catch (const char* e) {}
							catch (exception e) {}
							if (amount > 0)
							{
								mainHero->curHealth = amount;
								if (mainHero->maxHealth < mainHero->curHealth)
									mainHero->maxHealth = amount;
							}
						}
						else if (command == "set_max_health")
						{
							command.clear();
							while ((c = getchar()) != '\n')
								command += c;
							int amount;
							try
							{
								amount = stoi(command.c_str());
							}
							catch (const char* e) {}
							catch (exception e) {}
							if (amount > 0)
							{
								mainHero->maxHealth = amount;
								if (mainHero->curHealth > mainHero->maxHealth)
									mainHero->curHealth = amount;
							}
						}
						else if (command == "set_energy")
						{
							command.clear();
							while ((c = getchar()) != '\n')
								command += c;
							int amount;
							try
							{
								amount = stoi(command.c_str());
							}
							catch (const char* e) {}
							catch (exception e) {}
							if (amount > 0)
							{
								mainHero->actionsLeft = amount;
								if (mainHero->maxActionsCount < mainHero->actionsLeft)
									mainHero->maxActionsCount = amount;
							}
						}
						else if (command == "set_max_energy")
						{
							command.clear();
							while ((c = getchar()) != '\n')
								command += c;
							int amount;
							try
							{
								amount = stoi(command.c_str());
							}
							catch (const char* e) {}
							catch (exception e) {}
							if (amount > 0)
							{
								mainHero->maxActionsCount = amount;
								if (mainHero->actionsLeft > mainHero->maxActionsCount)
									mainHero->actionsLeft = amount;
							}
						}
						else if (command == "add_damage")
						{
							command.clear();
							while ((c = getchar()) != '\n')
								command += c;
							int amount;
							try
							{
								amount = stoi(command.c_str());
							}
							catch (const char* e) {}
							catch (exception e) {}
							mainHero->damage += amount;
						}
						else if (command == "set_sight_dist")
						{
							command.clear();
							while ((c = getchar()) != '\n')
								command += c;
							int amount;
							try
							{
								amount = stoi(command.c_str());
							}
							catch (const char* e) {}
							catch (exception e) {}
							if (amount > 0)
								mainHero->sightDistance = amount;
						}
						else if (command == "set_view_dist")
						{
							command.clear();
							while ((c = getchar()) != '\n')
								command += c;
							int amount;
							try
							{
								amount = stoi(command.c_str());
							}
							catch (const char* e) {}
							catch (exception e) {}
							if (amount > 0)
								mainHero->viewDistance = amount;
						}
					}
					else
					{
						if (command == "god_mode")
							godMode = !godMode;
					}
				}

				if (_event->key.keysym.sym == SDLK_PAGEUP) Time::SetTimeScale(Time::GetTimeScale() + 0.1);
				if (_event->key.keysym.sym == SDLK_PAGEDOWN) Time::SetTimeScale(Time::GetTimeScale() - 0.1);
				RecalculateOverlay();
				break;
			}
			break;
		}
	}
}

bool GameController::ScreenFade(bool fadeIn, double maxFadeState, double fadeSpeed, double *curFadeState)
{
	if (fadeIn)
	{
		if (*curFadeState < 1)
			*curFadeState = 1;
		if (*curFadeState > maxFadeState)
		{
			SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, maxFadeState);
			SDL_RenderFillRect(Initializer.renderer, nullptr);
			return true;
		}
		else if(*curFadeState > 255)
		{
			SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, 255);
			SDL_RenderFillRect(Initializer.renderer, nullptr);
			return true;
		} 
		else if (*curFadeState > 0)
			SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, *curFadeState);
		SDL_RenderFillRect(Initializer.renderer, nullptr);
		*curFadeState += fadeSpeed;
	}
	else
	{
		if (*curFadeState > 255)
			*curFadeState = 255;
		if (*curFadeState > maxFadeState)
			*curFadeState = maxFadeState;
		if (*curFadeState <= 0)
			return true;
		SDL_SetRenderDrawColor(Initializer.renderer, 0, 0, 0, *curFadeState);
		SDL_RenderFillRect(Initializer.renderer, nullptr);
		*curFadeState -= fadeSpeed;
	}

	return false;
}
bool GameController::TextFade(TextsContainer *text, bool fadeIn, double fadeSpeed, double *curFadeState)
{
	if (text == nullptr)
		return true;
	if (fadeIn)
	{
		if (*curFadeState < 1)
			*curFadeState = 1;
		if (*curFadeState > 255)
		{
			SDL_RenderSetScale(Initializer.renderer, 1, 1);
			text->Render(255);
			SDL_RenderSetScale(Initializer.renderer, (float)Controller.map->zoom, (float)Controller.map->zoom);
			return true;
		}
		SDL_RenderSetScale(Initializer.renderer, 1, 1);
		if (*curFadeState > 0)
			text->Render(*curFadeState);
		SDL_RenderSetScale(Initializer.renderer, (float)Controller.map->zoom, (float)Controller.map->zoom);
		*curFadeState += fadeSpeed;
	}
	else
	{
		if (*curFadeState > 255)
			*curFadeState = 255;
		if (*curFadeState <= 0)
			return true;
		SDL_RenderSetScale(Initializer.renderer, 1, 1);
		text->Render(*curFadeState);
		SDL_RenderSetScale(Initializer.renderer, (float)Controller.map->zoom, (float)Controller.map->zoom);
		*curFadeState -= fadeSpeed;
	}

	return false;
}

void GameController::StartGame(bool regenerateSeed, unsigned long long _sessionSeed)
{
	if (regenerateSeed)
		sessionSeed = chrono::system_clock::now().time_since_epoch().count();
	else
		sessionSeed = _sessionSeed;
	srand(sessionSeed);

	creaturesCount = 40 + 7 * sessionDay;//25 + rand() % 50
	if (creaturesCount > 70)
		creaturesCount = 70;
	shardsCount = 0;
	playerHealthbarPosition->x = playerHealthbarFullPosition->x = 8;
	playerHealthbarPosition->y = playerHealthbarFullPosition->y = 16;
	playerHealthbarPosition->w = playerHealthbarFullPosition->w = 104;
	playerHealthbarPosition->h = playerHealthbarFullPosition->h = 16;

	playerEnergybarPosition->x = playerEnergybarFullPosition->x = 8;
	playerEnergybarPosition->y = playerEnergybarFullPosition->y = 33;
	playerEnergybarPosition->w = playerEnergybarFullPosition->w = 104;
	playerEnergybarPosition->h = playerEnergybarFullPosition->h = 16;

	playerHealthbarEmptySrc->w = playerEnergybarSrc->w = playerHealthbarSrc->w = playerHealthbarForegroundSrc->w = 104;
	playerHealthbarEmptySrc->h = playerEnergybarSrc->h = playerHealthbarSrc->h = playerHealthbarForegroundSrc->h = 16;
	playerHealthbarEmptySrc->x = playerEnergybarSrc->x = playerHealthbarSrc->x = playerHealthbarForegroundSrc->x = 0;
	playerHealthbarEmptySrc->y = 16 * 2;
	playerHealthbarSrc->y = 16;
	playerHealthbarForegroundSrc->y = 0;
	playerEnergybarSrc->y = 16 * 3;

	map->PrepareMap();
	Overlay.Initialize();
	Inventory.Initialize();
	sessionMaxPossibleExploredTilesCount += map->explorableTilesCount;

	/*thread generationThread([&]()
		{
			Controller.map->PrepareMap();
		});
	generationThread.detach();*/

	/*SDL_Rect loadingBarRect =
	{
		Initializer.SCREEN_WIDTH / 4,
		Initializer.SCREEN_HEIGHT / 2 - 50,
		Initializer.SCREEN_WIDTH / 2,
		100
	};
	while (Controller.map->loadingPercent <= 99)
	{
		cout << Controller.map->loadingPercent << endl;
		SDL_RenderClear(Initializer.renderer);
		SDL_RenderCopy(Initializer.renderer, playerHealthbarTexture, playerHealthbarSrc, &loadingBarRect);
		SDL_RenderPresent(Initializer.renderer);
	}*/

	Tile* spawnTile;

	int shardsAvailable = 0;
	Creature* newCreature;
	for (int i = 0; i < creaturesCount; i++)
	{
		if (i != 0)
		{
			spawnTile = map->GetTileForCreatureSpawn();
			if (spawnTile == nullptr)
				continue;
			//creaturesQueue[i] = new CreatureElderEvilEye();
			int _rand = rand() % 1000;
			if (_rand > 500)
				newCreature = new CreatureGhost();
			else if (_rand > 180) newCreature = new CreatureEvilEye();
			else if (_rand > 50) newCreature = new CreatureBroodmother();
			else if (_rand > 10) newCreature = new CreatureElderEvilEye();
			else newCreature = new CreatureBlackWidow();
			newCreature->x = spawnTile->x;
			newCreature->y = spawnTile->y;
			newCreature->visualX = Initializer.TEXTURE_RES * -newCreature->x;
			newCreature->visualY = Initializer.TEXTURE_RES * -newCreature->y;
			newCreature->CalculateAnimationCoordinates(
				newCreature->visualX,
				newCreature->visualY);
			newCreature->CalculateVisualCoordinates(true);
			newCreature->isUnderPlayerControl = false;
			newCreature->factionID = 1;
		}
		else
		{
			if ((spawnTile = map->GetTileForPlayerSpawn()) == nullptr)
			{
				gameIsRunning = false;
				break;
			}
			newCreature = mainHero = new Hero();
			//creatures[i]->x = 5;//map->mapSizeX / 2;
			//creatures[i]->y = 5;//map->mapSizeY / 2;
			newCreature->x = spawnTile->x;
			newCreature->y = spawnTile->y;
			newCreature->visualX = Initializer.TEXTURE_RES * -newCreature->x;
			newCreature->visualY = Initializer.TEXTURE_RES * -newCreature->y;
			newCreature->isUnderPlayerControl = true;
			newCreature->factionID = 0;
			newCreature->endedTurn = false;
			newCreature->dropShardsCount = 0;
		}
		newCreature->map = map;
		newCreature->isAlive = true;
		newCreature->Initialize(i);
		shardsAvailable += newCreature->dropShardsCount;
		creaturesQueue.push_back(newCreature);
		creaturesIndexes.push_back(i);//creaturesIndexes[i] = i;
	}

	Tile* tile;
	for (int i = 0; i < 5;)
	{
		if (map->behindDoorTiles[i].size() < 1)
		{
			i++;
			continue;
		}
		int guardsCount = 1 + map->behindDoorTiles[i].size() / 18;
		int attemptsCount = 15;
		for (int j = 0; j < guardsCount && attemptsCount > 0;)
		{
			tile = (map->behindDoorTiles[i])[rand() % (3 * map->behindDoorTiles[i].size() / 4)];
			if (tile->GetTileObject() != nullptr)
			{
				attemptsCount--;
				continue;
			}

			attemptsCount = 15;

			if (Controller.sessionDay < 1)
			{
				int _rand = rand() % 1000;
				if (_rand > 100)
					newCreature = new CreatureEvilEye();
				else newCreature = new CreatureElderEvilEye();
			}
			else if (Controller.sessionDay < 2)
			{
				int _rand = rand() % 1000;
				if (_rand > 350)
					newCreature = new CreatureEvilEye();
				else if (_rand > 50) newCreature = new CreatureElderEvilEye();
				else newCreature = new CreatureBlackWidow();
			}
			else //if (Controller.sessionDay < 2)
			{
				int _rand = rand() % 1000;
				if (_rand > 400)
					newCreature = new CreatureElderEvilEye();
				else newCreature = new CreatureBlackWidow();
			}

			newCreature->x = tile->x;
			newCreature->y = tile->y;
			newCreature->visualX = Initializer.TEXTURE_RES * -newCreature->x;
			newCreature->visualY = Initializer.TEXTURE_RES * -newCreature->y;
			newCreature->CalculateAnimationCoordinates(
				newCreature->visualX,
				newCreature->visualY);
			newCreature->CalculateVisualCoordinates(true);
			newCreature->isUnderPlayerControl = false;
			newCreature->factionID = 1;
			newCreature->map = map;
			newCreature->isAlive = true;
			newCreature->Initialize(creaturesQueue.size() - 1);
			newCreature->curHealth = newCreature->maxHealth;
			shardsAvailable += newCreature->dropShardsCount;
			creaturesQueue.push_back(newCreature);
			creaturesIndexes.push_back(creaturesQueue.size() - 1);
			for (int x = newCreature->x - 1; x <= newCreature->x + 1; x++)
				for (int y = newCreature->y - 1; y <= newCreature->y + 1; y++)
					if ((tile = map->tilemap->GetTile(x, y)) != nullptr)
						tile->canSpawnCreature = false;
			j++;
		}
		i++;
	}

	for (auto i = map->doorTiles.begin(); i != map->doorTiles.end(); i++)
	{
		do
			tile = (*(map->biggestZone))[rand() % map->biggestZone->Count()];
		while (
			tile->doorIndex == (*i)->doorColor || 
			!tile->canPlaceKey ||
			tile->GetTileObject() != nullptr ||
			tile->tileItem != nullptr ||
			// prevent keys from spawning too close to hero
			Vector::GetDistance({ mainHero->x, mainHero->y }, { tile->x, tile->y }) < 20);

		int guardsCount = 0;
		for (int x = tile->x - 1; x <= tile->x + 1 && guardsCount < 4; x++)
			for (int y = tile->y - 1; y <= tile->y + 1 && guardsCount < 4; y++)
			{
				Tile* t = map->tilemap->GetTile(x, y);
				if (rand() % 100 < 40 || t == nullptr || t->tileType != 0 || t->GetTileObject() != nullptr)
					continue;

				int _rand = rand() % 1000;
				if (_rand > 450) newCreature = new CreatureEvilEye();
				else if (_rand > 100) newCreature = new CreatureBroodmother();
				else newCreature = new CreatureBlackWidow();

				newCreature->x = t->x;
				newCreature->y = t->y;
				newCreature->visualX = Initializer.TEXTURE_RES * -newCreature->x;
				newCreature->visualY = Initializer.TEXTURE_RES * -newCreature->y;
				newCreature->CalculateAnimationCoordinates(
					newCreature->visualX,
					newCreature->visualY);
				newCreature->CalculateVisualCoordinates(true);
				newCreature->isUnderPlayerControl = false;
				newCreature->factionID = 1;
				newCreature->map = map;
				newCreature->isAlive = true;
				newCreature->Initialize(creaturesQueue.size() - 1);
				newCreature->curHealth = newCreature->maxHealth;
				shardsAvailable += newCreature->dropShardsCount;
				creaturesQueue.push_back(newCreature);
				creaturesIndexes.push_back(creaturesQueue.size() - 1);
				guardsCount++;
			}

		Item* item = Items.GetItemByName((*i)->requiredItemIdNameToUnlock);
		tile->PlaceTileItem(new TileItem(
			{ tile->x, tile->y },
			item->textureSrc,
			Initializer.tilesetFloor,
			item->itemIdName,
			[&](TileObject* receiver, string connectedItemIdName)
			{
				Inventory.AddItemToInventory(new Item(*Items.GetItemByName(connectedItemIdName)));
				//Creature::Heal(25, (Creature*)receiver, nullptr);
			}
		));
	}

	/*{
		Tile* t = map->tilemap->GetTile(mainHero->x + 1, mainHero->y);
		newCreature = new BossTest();

		newCreature->x = t->x;
		newCreature->y = t->y;
		newCreature->visualX = Initializer.TEXTURE_RES * -newCreature->x;
		newCreature->visualY = Initializer.TEXTURE_RES * -newCreature->y;
		newCreature->CalculateAnimationCoordinates(
			newCreature->visualX,
			newCreature->visualY);
		newCreature->CalculateVisualCoordinates(true);
		newCreature->isUnderPlayerControl = false;
		newCreature->factionID = 1;
		newCreature->map = map;
		newCreature->isAlive = true;
		newCreature->Initialize(creaturesQueue.size() - 1);
		newCreature->curHealth = newCreature->maxHealth;
		shardsAvailable += newCreature->dropShardsCount;
		creaturesQueue.push_back(newCreature);
		creaturesIndexes.push_back(creaturesQueue.size() - 1);
	}*/

	Items.RecalculateItems();
	for (int i = 0; i < 5;)
	{
		if (map->behindDoorTiles[i].size() < 1)
		{
			i++;
			continue;
		}
		for (int j = 0; j < 5;)
		{
			int tileIndex;

			Item* item;
			if (j == 0)
			{
				int guardsInRoom = 1 + map->behindDoorTiles[i].size() / 18;
				tileIndex = map->behindDoorTiles[i].size() / 2 + rand() % (map->behindDoorTiles[i].size() / 2 - 2);
				vector<string> possibleLoot;

				while (possibleLoot.size() < 1)
				{
					if (Inventory.FindItem("lifestone_pendant"))
					{
						if (rand() % 100 < 40)
							possibleLoot.push_back("lifestone_pendant");
					}
					else possibleLoot.push_back("lifestone_pendant");

					if (Inventory.FindItem("lifestone_necklace"))
					{
						if (rand() % 100 < 40)
							possibleLoot.push_back("lifestone_necklace");
					}
					else possibleLoot.push_back("lifestone_necklace");
					if (sessionDay < 3 && guardsInRoom <= 3)
					{
						if (Inventory.FindItem("steel_shield"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("steel_shield");
						}
						else possibleLoot.push_back("steel_shield");

						if (Inventory.FindItem("steel_spear"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("steel_spear");
						}
						else possibleLoot.push_back("steel_spear");

						if (Inventory.FindItem("war_axe"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("war_axe");
						}
						else possibleLoot.push_back("war_axe");
					}
					if (sessionDay >= 1)
					{
						if (Inventory.FindItem("amethyst_shield"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("amethyst_shield");
						}
						else possibleLoot.push_back("amethyst_shield");

						if (Inventory.FindItem("bloodcall_mace"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("bloodcall_mace");
						}
						else possibleLoot.push_back("bloodcall_mace");

						if (Inventory.FindItem("soulburning_axe"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("soulburning_axe");
						}
						else possibleLoot.push_back("soulburning_axe");

						if (Inventory.FindItem("lightning_rod"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("lightning_rod");
						}
						else possibleLoot.push_back("lightning_rod");
					}
					if (sessionDay >= 2)
					{
						if (Inventory.FindItem("sunlight_shield"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("sunlight_shield");
						}
						else possibleLoot.push_back("sunlight_shield");

						if (Inventory.FindItem("amethyst_scythe"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("amethyst_scythe");
						}
						else possibleLoot.push_back("amethyst_scythe");

						if (Inventory.FindItem("lesser_astral_rod"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("lesser_astral_rod");
						}
						else possibleLoot.push_back("lesser_astral_rod");
					}
					if (sessionDay >= 3)
					{
						if (Inventory.FindItem("astral_greatsword"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("astral_greatsword");
						}
						else possibleLoot.push_back("astral_greatsword");
					}
					if (sessionDay >= 4)
					{
						if (Inventory.FindItem("starlight"))
						{
							if (rand() % 100 < 40)
								possibleLoot.push_back("starlight");
						}
						else possibleLoot.push_back("starlight");
					}
				}
				item = Items.GetRandomItemNonPercent(possibleLoot);
			}
			else
			{
				tileIndex = rand() % map->behindDoorTiles[i].size();
				item = Items.GetRandomItemNonPercent(
					{
						"medium_healing_potion",
						"big_healing_potion",
						"medium_energy_potion",
						"big_energy_potion",
						"fresh_apple",
						"chicken_leg"
					});
			}
			tile = (map->behindDoorTiles[i])[tileIndex];
			if (tile->tileItem != nullptr)
				continue;
			tile->PlaceTileItem(new TileItem(
				{ tile->x, tile->y },
				item->textureSrc,
				Initializer.tilesetFloor,
				item->itemIdName,
				[&](TileObject* receiver, string connectedItemIdName)
				{
					Inventory.AddItemToInventory(new Item(*Items.GetItemByName(connectedItemIdName)));
					//Creature::Heal(25, (Creature*)receiver, nullptr);
				}
			));
			if (item->itemIdName == "starlight" || item->itemIdName == "astral_greatsword")
			{
				int guardsCount = 0;
				for (int x = tile->x - 1; x <= tile->x + 1 && guardsCount < 2; x++)
					for (int y = tile->y - 1; y <= tile->y + 1 && guardsCount < 2; y++)
					{
						Tile* t = map->tilemap->GetTile(x, y);
						if (rand() % 100 < 40 || t == nullptr || t->tileType != 0 || t->GetTileObject() != nullptr)
							continue;

						newCreature = new CreatureBlackWidow();
						newCreature->x = t->x;
						newCreature->y = t->y;
						newCreature->visualX = Initializer.TEXTURE_RES * -newCreature->x;
						newCreature->visualY = Initializer.TEXTURE_RES * -newCreature->y;
						newCreature->CalculateAnimationCoordinates(
							newCreature->visualX,
							newCreature->visualY);
						newCreature->CalculateVisualCoordinates(true);
						newCreature->isUnderPlayerControl = false;
						newCreature->factionID = 1;
						newCreature->map = map;
						newCreature->isAlive = true;
						newCreature->Initialize(creaturesQueue.size() - 1);
						newCreature->maxHealth *= 2;
						newCreature->curHealth = newCreature->maxHealth;
						shardsAvailable += newCreature->dropShardsCount;
						creaturesQueue.push_back(newCreature);
						creaturesIndexes.push_back(creaturesQueue.size() - 1);
						guardsCount++;
					}
			}
			j++;
		}
		i++;
	}

	for (int i = 0; i < 55;)
	{
		tile = (*(map->biggestZone))[rand() % map->biggestZone->Count()];
		if (tile->tileItem != nullptr || tile->GetTileObject() != nullptr || !tile->canPlaceItem)
			continue;

		Item* item;
		int maxGuardsCount = 0;
		if (i == 0)
		{
			if (sessionDay < 3)
			{
				item = Items.GetRandomItemNonPercent(
					{
						"wooden_shield"
					});
				maxGuardsCount = 2;
			}
			else if (sessionDay < 6)
			{
				item = Items.GetRandomItemNonPercent(
					{
						"steel_shield"
					});
				maxGuardsCount = 3;
			}
			else// if (sessionDay < 12)
			{
				item = Items.GetRandomItemNonPercent(
					{
						"amethyst_shield"
					});
				maxGuardsCount = 4;
			}
		}
		else if (i < 3)
		{
			if (sessionDay > 1)
			{
				item = Items.GetRandomItemNonPercent(
					{
						"steel_sword",
						"war_axe",
						"steel_spear"
					});
				maxGuardsCount = 3;
			}
			else
			{
				item = Items.GetRandomItemNonPercent(
					{
						"small_dagger",
						"wooden_stick",
						"wooden_club",
						"steel_sword"
					});
				maxGuardsCount = 2;
			}
		}
		else if (i == 3)
		{
			item = Items.GetRandomItemNonPercent(
				{
					"lifestone_ring"
				});
			maxGuardsCount = 2;
		}
		else
		{
			item = Items.GetRandomItem(
				{
					"small_healing_potion",
					"medium_healing_potion",
					"big_healing_potion",
					"small_energy_potion",
					"medium_energy_potion",
					"big_energy_potion",
					"jar_lightning",
					"fresh_apple",
					"chicken_leg",
				});
			maxGuardsCount = rand() % 3;
		}

		if (Vector::GetDistance({ tile->x, tile->y }, mainHero->ToVector()) > 6)
		{
			int guardsCount = 0;
			for (int x = tile->x - 1; x <= tile->x + 1 && guardsCount < maxGuardsCount; x++)
				for (int y = tile->y - 1; y <= tile->y + 1 && guardsCount < maxGuardsCount; y++)
				{
					Tile* t = map->tilemap->GetTile(x, y);
					if (rand() % 100 < 40 || t == nullptr || t->tileType != 0 || t->GetTileObject() != nullptr)
						continue;

					int _rand = rand() % 1000;
					if (_rand > 350)
						newCreature = new CreatureGhost();
					else if (_rand > 80) newCreature = new CreatureEvilEye();
					else if (_rand > 6) newCreature = new CreatureBroodmother();
					else newCreature = new CreatureElderEvilEye();

					newCreature->x = t->x;
					newCreature->y = t->y;
					newCreature->visualX = Initializer.TEXTURE_RES * -newCreature->x;
					newCreature->visualY = Initializer.TEXTURE_RES * -newCreature->y;
					newCreature->CalculateAnimationCoordinates(
						newCreature->visualX,
						newCreature->visualY);
					newCreature->CalculateVisualCoordinates(true);
					newCreature->isUnderPlayerControl = false;
					newCreature->factionID = 1;
					newCreature->map = map;
					newCreature->isAlive = true;
					newCreature->Initialize(i);
					newCreature->curHealth = newCreature->maxHealth;
					shardsAvailable += newCreature->dropShardsCount;
					creaturesQueue.push_back(newCreature);
					creaturesIndexes.push_back(creaturesQueue.size() - 1);
					guardsCount++;
				}
		}
		
		tile->PlaceTileItem(new TileItem(
			{ tile->x, tile->y },
			item->textureSrc,
			Initializer.tilesetFloor,
			item->itemIdName,
			[&](TileObject* receiver, string connectedItemIdName)
			{
				Inventory.AddItemToInventory(new Item(*Items.GetItemByName(connectedItemIdName)));
				//Creature::Heal(25, (Creature*)receiver, nullptr);
			}
		));
		Tile* t;
		for (int x = tile->x - 1; x <= tile->x + 1; x++)
			for (int y = tile->y - 1; y <= tile->y + 1; y++)
				if ((t = map->tilemap->GetTile(x, y)) != nullptr)
					t->canPlaceItem = false;
		i++;
	}

	shardsRequired = shardsAvailable / 2;
	altar = nullptr;
	for (int x = mainHero->x - 1; x <= mainHero->x + 1 && altar == nullptr; x++)
		for (int y = mainHero->y - 1; y <= mainHero->y + 1 && altar == nullptr; y++)
		{
			tile = map->tilemap->GetTile(x, y);
			if (tile != nullptr &&
				tile->tileType == 0 &&
				tile->GetTileObject() == nullptr &&
				tile->tileItem == nullptr &&
				y > 0 && y < map->mapSizeY &&
				x > 0 && x < map->mapSizeX &&
				map->tilemap->GetTile(x, y - 1)->tileType == 0 &&
				map->tilemap->GetTile(x, y + 1)->tileType == 0 &&
				map->tilemap->GetTile(x - 1, y)->tileType == 0 &&
				map->tilemap->GetTile(x + 1, y)->tileType == 0)
			{
				altar = new ObjectAltar();
				altar->x = x;
				altar->y = y;
				altar->visualX = Initializer.TEXTURE_RES * -x;
				altar->visualY = Initializer.TEXTURE_RES * -y;
				altar->CalculateAnimationCoordinates(
					altar->visualX,
					altar->visualY);
				altar->CalculateVisualCoordinates(true);
				altar->map = map;
				altar->Initialize(creaturesCount);
				tile->SetTileObject(altar);
			}
		}

	for (int i = 0; i < 5; i++)
		map->behindDoorTiles[i].clear();
	delete[] map->behindDoorTiles;

	RecalculateRenderLayers();
	currentCreature = mainHero;
	currentCreature->endedTurn = true;
	map->camera.target = currentCreature;
	map->camera.CameraTeleport(map);
	currentCreature->StartTurn();
	MapHandler::Instance().Initialize();
	MapHandler::Instance().RecalculateFogOfWar(map,
		map->tilemap->GetTile(currentCreature->x, currentCreature->y),
		currentCreature->viewDistance);
	MapHandler::Instance().RecalculateVisibleLands(map,
		map->tilemap->GetTile(currentCreature->x, currentCreature->y),
		currentCreature->sightDistance);
	RecalculateOverlay();
}

void GameController::InitializeOverlayAndInventory()
{
	Overlay.Initialize();
	Inventory.Initialize();
}

void GameController::WriteInventoryInFile(FILE* writeTo)
{
	int curID;
	for (int i = 0; i < Inventory.slots.size(); i++)
	{
		curID = 0;
		Item* item = Inventory.slots[i]->GetItem();
		if (item != nullptr)
			curID = item->itemID;
		fwrite(&curID, sizeof(int), 1, writeTo);
	}
}

void GameController::ReadInventoryFromFile(FILE* readFrom)
{
	int curID;
	for (int i = 0; i < Inventory.slots.size(); i++)
	{
		fread(&curID, sizeof(int), 1, readFrom);
		Item* foundInDatabase = Items.GetItemByID(curID);
		if (curID > 0 && foundInDatabase != nullptr && foundInDatabase->transferToNextDay)
			Inventory.AddItemToSlot(new Item(*foundInDatabase), Inventory.slots[i]);
	}
	mainHero->curHealth = mainHero->maxHealth;
	RecalculateOverlay();
}

/*void ReadInventoryFromString(string readFrom)
{
	int curID;
	for (int i = 0; i < Inventory.slots.size(); i++)
	{
		cout << curID << endl;
		if (curID > 0)
			Inventory.AddItemToSlot(Items.GetItemByID(curID), Inventory.slots[i]);
	}
}*/

int GameController::main_copy(int argc, char *argv[])
{
	map = new MapController();

	if (!Initializer.Initialize() || !Initializer.Load())
	{
		delete map;
		Initializer.QuitGame();
		return 1;
	}

	splashScreenText = new TextsContainer(1, 
		new Text[1]
		{
			Text("fonts/pixel_font1.ttf", L"День 1", 100, 20, Text::AlignMode::ToMiddle, { 255, 255, 255}),
		});
	splashScreenText->Initialize(
		Initializer.SCREEN_WIDTH / 2,
		Initializer.SCREEN_HEIGHT / 2 - splashScreenText->GetHeight() / 2);

	firstDaySplashText = new TextsContainer(6, 
		new Text[6]
		{
			Text("fonts/pixel_font1.ttf", L"- Перемещайтесь и исследуйте территории", 100, 20, Text::AlignMode::ToLeft, { 255, 255, 255}),
			Text("fonts/pixel_font1.ttf", L"- Ищите ценные предметы и оружие", 100, 20, Text::AlignMode::ToLeft, { 255, 255, 255}),
			Text("fonts/pixel_font1.ttf", L"- Накопите осколки для перехода на новый день", 100, 20, Text::AlignMode::ToLeft, { 255, 255, 255}),
			Text("fonts/pixel_font1.ttf", L"", 100, 20, Text::AlignMode::ToLeft, { 255, 255, 255}),
			Text("fonts/pixel_font1.ttf", L"Продержитесь как можно дольше", 100, 20, Text::AlignMode::ToMiddle, { 255, 255, 255}),
			Text("fonts/pixel_font1.ttf", L"Удачи", 100, 20, Text::AlignMode::ToMiddle, { 255, 0, 0})
		});
	firstDaySplashText->Initialize(
		Initializer.SCREEN_WIDTH / 2,
		Initializer.SCREEN_HEIGHT / 2 - firstDaySplashText->GetHeight() / 2);

	gameOverSplashText = new TextsContainer(2, 
		new Text[2]
		{
			Text("fonts/pixel_font1.ttf", L"Игра окончена", 150, 20, Text::AlignMode::ToMiddle, { 255, 0, 0}),
			Text("fonts/pixel_font1.ttf", L"Ваш герой погиб", 100, 20, Text::AlignMode::ToMiddle, { 255, 255, 255})
		});
	gameOverSplashText->Initialize(
		Initializer.SCREEN_WIDTH / 2,
		Initializer.SCREEN_HEIGHT / 2 - gameOverSplashText->GetHeight() / 2);

	Items.Initialize();
	tileSelectionSrc = { 7 * Initializer.TEXTURE_RES, 2 * Initializer.TEXTURE_RES, 16, 16 };
	tileSelectionWrongSrc = { 8 * Initializer.TEXTURE_RES, 2 * Initializer.TEXTURE_RES, 16, 16 };
	while (!quitGame)
	{
		//InitializeOverlayAndInventory();
		Menu.Initialize();

		gameIsRunning = true;
		gameInPause = false;
		gameState = GameState::InMenu;
		Menu.currentState = Menu.GetStateByName("main_menu");

		while (Update());

		if (altar != nullptr)
		{
			delete altar;
			altar = nullptr;
		}
		//if (/*creatures*/creaturesQueue != nullptr)
		{
			/*for (int i = 0; i < map->mapSizeY; i++)
				for (auto j = creatures[i].begin(); j != creatures[i].end(); j++)
					delete *j;*/
			for (int i = 0; i < creaturesQueue.size()/*creaturesCount*/; i++)
				if (creaturesQueue[i] != nullptr)
				{
					delete creaturesQueue[i];
					creaturesQueue[i] = nullptr;
				}

			creaturesQueue.clear();
			creaturesIndexes.clear();
			//delete /*creaturesQueue, */creaturesIndexes;
			//creaturesQueue = nullptr;
			//creaturesIndexes = nullptr;
		}

		delete map;
		map = new MapController();
		Menu.DestroyMenu();
	}

	delete[] splashScreenText->texts;
	delete splashScreenText;

	delete[] firstDaySplashText->texts;
	delete firstDaySplashText;

	delete[] gameOverSplashText->texts;
	delete gameOverSplashText;
	if (map != nullptr)
		delete map;
	Initializer.QuitGame();
	return 0;
}

int main(int argc, char *argv[])
{
	// transferring control to the copy of the main function
	Controller.main_copy(argc, argv);
	return 0;
}