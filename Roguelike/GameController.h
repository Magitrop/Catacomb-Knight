#pragma once
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <SDL_ttf.h>
#include <functional>
#include <vector>
#include "Time.h"
#include "Text.h"
#include "MapController.h"
#include "Creature.h"
#include "VisualEffect.h"

class GameController final
{
private:
	GameController() {}
	~GameController() {}
	GameController(GameController& gc) {}
	GameController& operator = (GameController& gc) {}
public:
	static GameController& Instance()
	{
		static GameController instance;
		return instance;
	}

	class SplashScreen final
	{
	private:
		double fadeInDuration;
		double fadeOutDuration;
		double textInDuration;
		double textOutDuration;

		double maxFadeState;
		double curFadeState;
		double curTextFadeState;
		double innerWaitingTimer;
	public:
		enum class SplashScreenState
		{
			Preparation,
			FadingIn,
			TextFadingIn,
			TextFadingOut,
			Waiting,
			FadingOut,
			Done
		} state;

		double waitingDuration;
		TextsContainer* text;
		function<void()> onFullFadeIn;
		function<void()> onFullFadeOut;
		function<void()> onWaitingEnd;
		function<void()> onFullTextIn;
		function<void()> onFullTextOut;

		SplashScreen(double _maxFadeState, double _fadeIn, double _fadeOut, double _waiting, double _textIn, double _textOut, TextsContainer *_text);
		// returns true when full cycle is done
		bool Play();
	};

	string saveFileName;
	unsigned long long sessionSeed;
	wstring sessionName;
	int sessionDay;
	int sessionExploredTilesCount;
	int sessionMaxPossibleExploredTilesCount;

	TextsContainer* splashScreenText;
	TextsContainer* firstDaySplashText;
	TextsContainer* gameOverSplashText;

	double fadeState = 0;
	double splashScreenTimer = 0;
	double titleFadeState = 0;

	MapController *map;
	SDL_Event eventHandler;
	bool quitGame = false;
	bool gameIsRunning = true;
	bool gameInPause = false;
	bool isSelectingTile = false;
	bool inSplashScreen = false;
	bool godMode = false;
	double curPause = 0;

	int shardsCount = 0;
	int shardsRequired;

	vector<VisualEffect*> visualEffects;

	int creaturesCount = 100;
	int currentCreatureIndex = 0;
	vector<int> creaturesIndexes;
	//int* creaturesIndexes;
	//vector<Creature*>* creatures;
	//Creature** creaturesQueue;
	vector<Creature*> creaturesQueue;
	Creature* currentCreature;
	Creature* mainHero;
	TileObject* altar;

	enum class GameState
	{
		InMenu,
		InGame
	} gameState;

	SDL_Texture *playerHealthbarTexture;
	SDL_Rect *playerHealthbarEmptySrc = new SDL_Rect();
	SDL_Rect* playerHealthbarSrc = new SDL_Rect();
	SDL_Rect *playerHealthbarForegroundSrc = new SDL_Rect();
	SDL_Rect *playerHealthbarPosition = new SDL_Rect();
	SDL_Rect *playerHealthbarFullPosition = new SDL_Rect();

	SDL_Rect* playerEnergybarSrc = new SDL_Rect();
	SDL_Rect* playerEnergybarPosition = new SDL_Rect();
	SDL_Rect* playerEnergybarFullPosition = new SDL_Rect();

	int mouseX, mouseY;
	Tile* currentTileSelection;
	SDL_Rect tileSelectionSrc;
	SDL_Rect tileSelectionWrongSrc;

	SplashScreen *splashScreen;

	void WriteInventoryInFile(FILE* writeTo);
	void ReadInventoryFromFile(FILE* readFrom);
	void InitializeOverlayAndInventory(void);
	//void ReadInventoryFromString(string readFrom);

	void SetPause(double pause);
	double GetPause(void);
	bool Update(void);
	void NextTurn(void);
	void NextDay(void);
	void BeginTileSelection();
	void EndTileSelection(Tile* selectedTile);
	void RecalculateOverlay(void);
	void RecalculateRenderLayers(void);
	void EventHandle(SDL_Event *_event);
	bool ScreenFade(bool fadeIn, double maxFadeState, double fadeSpeed, double* curFadeState);
	bool TextFade(TextsContainer* text, bool fadeIn, double fadeSpeed, double* curFadeState);
	void StartGame(bool newSeed, unsigned long long _sessionSeed);
	int main_copy(int argc, char** args);
};