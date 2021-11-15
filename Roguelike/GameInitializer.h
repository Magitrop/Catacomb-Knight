#pragma once
#include <SDL.h>
#include <SDL_image.h>
using namespace std;

class GameInitializer final
{
private:
	GameInitializer(void) {}
	GameInitializer(const GameInitializer& gc) {}
	GameInitializer& operator = (GameInitializer& gc) {}
	~GameInitializer(void) {}
public:
	static GameInitializer& Instance()
	{
		static GameInitializer instance;
		return instance;
	}

	int SCREEN_WIDTH = 1380;
	int SCREEN_HEIGHT = 900;
	const int TEXTURE_RES = 16;
	const int MINIMAP_TILE_RES = 8;

	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	SDL_Texture *tilesetFloor = nullptr;
	SDL_Texture* tilesetWall = nullptr;

	bool Initialize(void);
	bool Load(void);
	void QuitGame(void);
};