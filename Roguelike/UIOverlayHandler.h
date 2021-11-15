#pragma once

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <typeindex>
#include <functional>
#include <filesystem>
#include "GameInitializer.h"
#include "GameController.h"
#include "MenuHandler.h"

using namespace std;

class UIOverlayHandler final
{
private:
	UIOverlayHandler() {}
	~UIOverlayHandler() 
	{
		SDL_DestroyTexture(textureUI);
	}
	UIOverlayHandler(UIOverlayHandler& mm) {}
	UIOverlayHandler& operator = (UIOverlayHandler& mm) {}

	int menuItemWidth;
	int menuItemHeight;
	int renderStartFromX;
	int renderStartFromY;
	int renderGapY;
	int borderMarginX;
	int borderMarginY;
public:
	SDL_Rect rectForOverlay;
	SDL_Texture* textureUI;

	static UIOverlayHandler& Instance()
	{
		static UIOverlayHandler instance;
		return instance;
	}

	vector<MenuItem*> topLeftCorner;
	vector<MenuItem*> topRightCorner;
	vector<MenuItem*> bottomLeftCorner;
	vector<MenuItem*> bottomRightCorner;

	void Initialize()
	{
		borderMarginX =  10;
		borderMarginY = 50;

		SDL_DestroyTexture(textureUI);
		auto surf = IMG_Load("textures/ui.png");
		textureUI = SDL_CreateTextureFromSurface(Initializer.renderer, surf);
		SDL_FreeSurface(surf);

		rectForOverlay = { 0, 0, 400, 10 };
	}
	void AddImageWithLabel(SDL_Rect imageRect, wstring labelText, int fontSize, SDL_Color color, vector<MenuItem*>& toVector)
	{
		auto image = new MenuItemImage("image",
			textureUI, imageRect, 35, 35, { 10, 35 });
		toVector.push_back((MenuItem*)image);
		auto label = new MenuItemLabel("label",
			new Text("fonts/pixel_font1.ttf", labelText, fontSize, 20, Text::AlignMode::ToMiddle, color));
		toVector.push_back((MenuItem*)label);
	}
	void AddLabel(wstring labelText, int fontSize, SDL_Color color, vector<MenuItem*>& toVector)
	{
		auto label = new MenuItemLabel("label",
			new Text("fonts/pixel_font1.ttf", labelText, fontSize, 20, Text::AlignMode::ToMiddle, color));
		toVector.push_back((MenuItem*)label);
	}
	void AddGap(vector<MenuItem*>& toVector)
	{
		auto gap = new MenuItemNone("gap");
		toVector.push_back((MenuItem*)gap);
	}
	void Calculate()
	{
		renderGapY = 10;
		menuItemWidth = rectForOverlay.w;
		menuItemHeight = rectForOverlay.h;

		renderStartFromX = borderMarginX;
		renderStartFromY = borderMarginY;
		rectForOverlay.x = renderStartFromX;
		rectForOverlay.y = renderStartFromY;
		for (auto i = topLeftCorner.begin(); i != topLeftCorner.end(); i++)
		{
			(*i)->renderRect = { rectForOverlay.x, rectForOverlay.y, menuItemWidth, menuItemHeight };
			rectForOverlay.y += menuItemHeight + renderGapY;
		}

		renderStartFromX = Initializer.SCREEN_WIDTH - menuItemWidth - borderMarginX;
		renderStartFromY = borderMarginY;
		rectForOverlay.x = renderStartFromX;
		rectForOverlay.y = renderStartFromY;
		for (auto i = topRightCorner.begin(); i != topRightCorner.end(); i++)
		{
			(*i)->renderRect = { rectForOverlay.x, rectForOverlay.y, menuItemWidth, menuItemHeight };
			rectForOverlay.y += menuItemHeight + renderGapY;
		}

		renderStartFromX = borderMarginX;
		renderStartFromY = Initializer.SCREEN_HEIGHT - bottomLeftCorner.size() * (renderGapY + menuItemHeight) - borderMarginY;
		rectForOverlay.x = renderStartFromX;
		rectForOverlay.y = renderStartFromY;
		for (auto i = bottomLeftCorner.begin(); i != bottomLeftCorner.end(); i++)
		{
			(*i)->renderRect = { rectForOverlay.x, rectForOverlay.y, menuItemWidth, menuItemHeight };
			rectForOverlay.y += menuItemHeight + renderGapY;
		}

		renderStartFromX = Initializer.SCREEN_WIDTH - menuItemWidth - borderMarginX + 25;
		renderStartFromY = Initializer.SCREEN_HEIGHT - bottomRightCorner.size() * (renderGapY + menuItemHeight) - borderMarginY;
		rectForOverlay.x = renderStartFromX;
		rectForOverlay.y = renderStartFromY;
		for (auto i = bottomRightCorner.begin(); i != bottomRightCorner.end(); i++)
		{
			(*i)->renderRect = { rectForOverlay.x, rectForOverlay.y, menuItemWidth, menuItemHeight };
			rectForOverlay.y += menuItemHeight + renderGapY;
		}
	}
	void Render()
	{
		for (auto i = topLeftCorner.begin(); i != topLeftCorner.end(); i++)
			(*i)->Render();
		for (auto i = topRightCorner.begin(); i != topRightCorner.end(); i++)
			(*i)->Render();
		for (auto i = bottomLeftCorner.begin(); i != bottomLeftCorner.end(); i++)
			(*i)->Render();
		for (auto i = bottomRightCorner.begin(); i != bottomRightCorner.end(); i++)
			(*i)->Render();
	}
	void ClearOverlay()
	{
		for (auto i = topLeftCorner.begin(); i != topLeftCorner.end(); i++)
			delete *i;
		topLeftCorner.clear();
		for (auto i = topRightCorner.begin(); i != topRightCorner.end(); i++)
			delete *i;
		topRightCorner.clear();
		for (auto i = bottomLeftCorner.begin(); i != bottomLeftCorner.end(); i++)
			delete *i;
		bottomLeftCorner.clear();
		for (auto i = bottomRightCorner.begin(); i != bottomRightCorner.end(); i++)
			delete *i;
		bottomRightCorner.clear();
	}
};