#define Controller GameController::Instance()

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <future>
#include "GameController.h"
#include "GameInitializer.h"
#include "MapHandler.h"
using namespace std;

bool GameInitializer::Initialize(void)
{
    bool ok = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        cout << "Can't init: " << SDL_GetError() << endl;
        ok = false;
    }

    window = SDL_CreateWindow("Game", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        cout << "Can't create window: " << SDL_GetError() << endl;
        ok = false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        cout << "Can't create renderer: " << SDL_GetError() << endl;
        ok = false;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    int flags = IMG_INIT_PNG;
    if (!(IMG_Init(flags) & flags))
    {
        cout << "Can't init image: " << IMG_GetError() << endl;
        ok = false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        ok = false;
    }

    TTF_Init();
    SDL_RenderSetScale(renderer, (float)Controller.map->zoom, (float)Controller.map->zoom);
    return ok;
}

bool GameInitializer::Load(void)
{
    bool ok = true;

    auto surf = IMG_Load("textures/tiles_floor.png");
    tilesetFloor = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    tilesetWall = SDL_CreateTextureFromSurface(renderer, surf = IMG_Load("textures/tiles_wall.png"));
    SDL_FreeSurface(surf);

    Controller.playerHealthbarTexture = SDL_CreateTextureFromSurface(renderer, surf = IMG_Load("textures/hero_ui.png"));
    SDL_FreeSurface(surf);

    return ok;
}
void GameInitializer::QuitGame(void)
{
    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    SDL_DestroyTexture(tilesetFloor);
    tilesetFloor = nullptr;

    SDL_DestroyTexture(tilesetWall);
    tilesetWall = nullptr;

    SDL_DestroyTexture(Controller.playerHealthbarTexture);
    delete Controller.playerHealthbarEmptySrc,
        Controller.playerHealthbarSrc,
        Controller.playerHealthbarForegroundSrc,
        Controller.playerHealthbarPosition,
        Controller.playerHealthbarFullPosition;

    Mix_Quit();
    IMG_Quit();
    //TTF_Quit();
    SDL_Quit();
}