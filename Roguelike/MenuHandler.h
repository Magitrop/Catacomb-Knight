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
#include "Text.h"

using namespace std;
using filesystem::directory_iterator;

class MenuItem
{
public:
	class ItemParameter final
	{
	public:
		string name;
		void* value;
		//const char* value;

		//template <class T>
		ItemParameter(string _name, void* _value) //: value(static_cast<char*>(static_cast<void*>(_value)))
		{
			name = _name;
			value = _value;
		}
		~ItemParameter()
		{
			delete value;
		}
	};
protected:
	vector<ItemParameter*> parameters;
public:
	SDL_Rect renderRect;
	bool mouseIn;
	string name;

	//template <class T>
	void AddParameter(string paramName, void* param)
	{
		auto parameter = new ItemParameter(paramName, param);
		parameters.push_back(parameter);
	}
	ItemParameter* GetParameterByName(string paramName)
	{
		for (auto i = parameters.begin(); i != parameters.end(); i++)
			if ((*i)->name == paramName)
				return *i;
		return nullptr;
	}

	virtual ~MenuItem() 
	{
		for (auto i = parameters.begin(); i != parameters.end(); i++)
			delete *i;
	}
	virtual void Render() = 0;
	virtual void OnMouseEnter() = 0;
	virtual void OnMouseOut() = 0;
};
class MenuState sealed
{
private:
	int menuItemWidth;
	int menuItemHeight;
	int renderStartFromX;
	int renderStartFromY;
	int renderGapY;
	string stateName;
public:
	vector<MenuItem*> items;
	SDL_Rect rectForState;
	MenuState* previousState;

	MenuState(string _stateName, SDL_Rect _rectForState)
	{
		stateName = _stateName;
		rectForState = _rectForState;
	}
	MenuState(MenuState& menuState)
	{
		stateName = menuState.stateName;
		items = menuState.items;
		rectForState = menuState.rectForState;
	}

	void Initialize()
	{
		renderGapY = 10;
		menuItemWidth = rectForState.w;
		menuItemHeight = rectForState.h;
		renderStartFromX = (Initializer.SCREEN_WIDTH - menuItemWidth) / 2;
		renderStartFromY = (Initializer.SCREEN_HEIGHT - items.size() * (renderGapY + menuItemHeight)) / 2;
		rectForState.x = renderStartFromX;
		rectForState.y = renderStartFromY;

		for (auto i = items.begin(); i != items.end(); i++)
		{
			(*i)->renderRect = { rectForState.x, rectForState.y, menuItemWidth, menuItemHeight };
			rectForState.y += menuItemHeight + renderGapY;
		}
	}
	void Render()
	{
		//rectForState.y = renderStartFromY;
		for (auto i = items.begin(); i != items.end(); i++)
			(*i)->Render();
		//rectForState.y += menuItemHeight + renderGapY;
		/*rectForState.w = (*i)->itemText.GetWidth();
		rectForState.h = (*i)->itemText.GetHeight();*/
	}
	void RemoveState()
	{
		for (auto i = items.begin(); i != items.end(); i++)
			delete *i;
	}

	string GetStateName()
	{
		return stateName;
	}
	static MenuItem* GetItemByName(MenuState* state, string name);
};

class MenuItemClickable abstract : public MenuItem
{
protected:
	class ClickAction final
	{
	public:
		function<void(MenuItem*)> action;
		MenuItem* caller;
		ClickAction(function<void(MenuItem*)> act, MenuItem* _caller)
		{
			action = act;
			caller = _caller;
		}
	};

	ClickAction* clickAction;
	//function<void(MenuItem*)> clickAction;
	vector<function<bool()>> blockingConditions;
public:
	bool isBlocked;

	virtual ~MenuItemClickable() override
	{
		if (clickAction != nullptr)
			delete clickAction;
	}

	void AddBlockingCondition(function<bool()> cond)
	{
		blockingConditions.push_back(cond);
	}
	void SetClickAction(function<void(MenuItem*)> _func, MenuItem* caller)
	{
		if (clickAction != nullptr)
			delete clickAction;
		clickAction = new ClickAction(_func, caller);
		//clickAction = _func;
	}
	void SetClickAction(function<void(MenuItem*)> _func)
	{
		if (clickAction != nullptr)
			delete clickAction;
		clickAction = new ClickAction(_func, this);
		//clickAction = _func;
	}

	// when this item is pressed
	void OnClick()
	{
		if (!isBlocked)
			clickAction->action(clickAction->caller);
	}
};

// menu empty space template
class MenuItemNone sealed : public MenuItem
{
public:
	MenuItemNone(string _name)
	{
		name = _name;
	}
	virtual ~MenuItemNone() override
	{

	}
	 
	virtual void Render() override
	{
		//cout << rect->x << " " << rect->y << " " << rect->w << " " << rect->h << endl;
	}
	virtual void OnMouseEnter() override
	{
		mouseIn = true;
	}
	virtual void OnMouseOut() override
	{
		mouseIn = false;
	}
};
// menu label template
class MenuItemLabel sealed : public MenuItem
{
private:
	Text* itemText;
public:
	MenuItemLabel(string _name, Text* _text)
	{
		name = _name;
		itemText = _text;
	}
	virtual ~MenuItemLabel() override
	{
		delete itemText;
	}

	virtual void Render() override
	{
		//cout << rect->x << " " << rect->y << " " << rect->w << " " << rect->h << endl;
		itemText->SetRenderCoordinates(renderRect.x + renderRect.w / 2, renderRect.y - renderRect.h / 4);
		itemText->Render(255);
	}
	virtual void OnMouseEnter() override
	{
		mouseIn = true;
	}
	virtual void OnMouseOut() override
	{
		mouseIn = false;
	}
};
// menu horizontal container template
class MenuItemHorizontalContainer sealed : public MenuItem
{
private:
	int menuItemWidth;
	int menuItemHeight;
	int renderStartFromX;
	int renderStartFromY;
	string containerName;
public:
	int renderGapX;
	vector<MenuItem*> items;

	MenuItemHorizontalContainer(string _containerName, SDL_Rect _rectForContainer)
	{
		renderGapX = 10;
		containerName = _containerName;
		renderRect = _rectForContainer;
	}

	virtual ~MenuItemHorizontalContainer() override
	{
		for (auto i = items.begin(); i != items.end(); i++)
		{
			if (dynamic_cast<MenuItemHorizontalContainer*>(*i))
			{
				auto cont = (MenuItemHorizontalContainer*)(*i);
				for (auto j = cont->items.begin(); j != cont->items.end(); j++)
					delete *j;
			}
			delete *i;
		}
	}

	void Initialize()
	{
		menuItemWidth = renderRect.w;
		menuItemHeight = renderRect.h;
		renderStartFromX = (Initializer.SCREEN_WIDTH - items.size() * (renderGapX + menuItemWidth) + renderGapX) / 2;
		renderRect.x = renderStartFromX;
	}
	void Initialize(int _startFromX)
	{
		menuItemWidth = renderRect.w;
		menuItemHeight = renderRect.h;
		renderStartFromX = renderRect.x = _startFromX;
	}
	virtual void Render() override
	{
		//renderStartFromX = (Initializer.SCREEN_WIDTH - (*i)->renderRect.w) / 2;
		renderRect.x = renderStartFromX;
		for (auto i = items.begin(); i != items.end(); i++)
		{
			(*i)->renderRect.x = renderRect.x;
			(*i)->renderRect.y = renderRect.y;
			(*i)->renderRect.w = renderRect.w;
			(*i)->renderRect.h = renderRect.h;
			(*i)->Render();
			renderRect.x += menuItemWidth + renderGapX;
		}
		/*itemText->SetRenderCoordinates(renderRect.x + renderRect.w / 2, renderRect.y - renderRect.h / 4);
		itemText->Render(255);*/
	}
	virtual void OnMouseEnter() override
	{
		mouseIn = true;
	}
	virtual void OnMouseOut() override
	{
		mouseIn = false;
	}
};
// menu button template
class MenuItemButton sealed : public MenuItemClickable
{
private:
	Text* itemText;
	SDL_Texture* textureUI;
	SDL_Rect srcIdle, srcSelected, srcBlocked;
public:
	MenuItemButton(string _name, Text* _text, SDL_Texture* _texture)
	{
		name = _name;
		itemText = _text;
		textureUI = _texture;
		srcIdle = { 0, 0, 300, 75 };
		srcSelected = { 0, 75, 300, 75 };
		srcBlocked = { 0, 75 * 2, 300, 75 };
	}
	virtual ~MenuItemButton() override
	{
		delete itemText;
	}

	virtual void Render() override
	{
		for (auto i = blockingConditions.begin(); i != blockingConditions.end(); i++)
			if ((*i)())
			{
				isBlocked = true;
				break;
			}
			else isBlocked = false;

		itemText->SetRenderCoordinates(renderRect.x + renderRect.w / 2, renderRect.y - renderRect.h / 4);
		if (mouseIn && !isBlocked)
		{
			SDL_RenderCopy(Initializer.renderer, textureUI, &srcSelected, &renderRect);
			itemText->Render(255);
		}
		else
		{
			if (!isBlocked)
				SDL_RenderCopy(Initializer.renderer, textureUI, &srcIdle, &renderRect);
			else
				SDL_RenderCopy(Initializer.renderer, textureUI, &srcBlocked, &renderRect);
			itemText->Render(150);
		}
	}
	virtual void OnMouseEnter() override
	{
		mouseIn = true;
	}
	virtual void OnMouseOut() override
	{
		mouseIn = false;
	}
};
// menu textbox template
class MenuItemTextbox sealed : public MenuItemClickable
{
private:
	Text* itemText;
	SDL_Texture* textureUI;
	SDL_Rect srcIdle, srcSelected;
	int maxTextLength;
public:
	bool isByDefault;

	MenuItemTextbox(string _name, Text* _text, SDL_Texture* _texture, int _maxTextLength)
	{
		name = _name;
		isByDefault = true;
		itemText = _text;
		textureUI = _texture;
		srcIdle = { 0, 0, 64, 32 };
		srcSelected = { 0, 32, 64, 32 };
		maxTextLength = _maxTextLength;
	}
	virtual ~MenuItemTextbox() override
	{
		delete itemText;
	}

	virtual void Render() override
	{
		//cout << rect->x << " " << rect->y << " " << rect->w << " " << rect->h << endl;
		itemText->SetRenderCoordinates(renderRect.x + renderRect.w / 2, renderRect.y - renderRect.h / 4);
		if (mouseIn || !isByDefault)
		{
			//SDL_RenderCopy(Initializer.renderer, textureUI, &srcSelected, &renderRect);
			itemText->Render(255);
		}
		else
		{
			//SDL_RenderCopy(Initializer.renderer, textureUI, &srcIdle, &renderRect);
			itemText->Render(150);
		}
	}
	virtual void OnMouseEnter() override
	{
		mouseIn = true;
	}
	virtual void OnMouseOut() override
	{
		mouseIn = false;
	}

	wstring GetValue()
	{
		return itemText->text;
	}

	void EnterCharacter(wchar_t c)
	{
		if (isByDefault)
		{
			itemText->text.clear();
			itemText->text.push_back(c);
			itemText->RebuildText();
			isByDefault = false;
		}
		else
		{
			if (itemText->text.length() < maxTextLength)
			{
				itemText->text.push_back(c);
				itemText->RebuildText();
			}
		}
	}
	void RemoveCharacter()
	{
		if (isByDefault)
			return;
		if (itemText->text.length() > 1)
		{
			itemText->text.pop_back();
			itemText->RebuildText();
		}
		else
			Clear();
	}
	void Clear()
	{
		isByDefault = true;
		itemText->text = L"введите имя";
		itemText->RebuildText();
	}
};
// menu image template
class MenuItemImage sealed : public MenuItem
{
private:
	SDL_Texture* itemImage;
	SDL_Rect src;
	SDL_Rect rect;
	int imageWidth, imageHeight;
	SDL_Point imageOffset;
public:
	MenuItemImage(string _name, SDL_Texture* texture, SDL_Rect _src, int width, int height)
	{
		name = _name;
		itemImage = texture;
		src = _src;
		imageWidth = width;
		imageHeight = height;
	}
	MenuItemImage(string _name, SDL_Texture* texture, SDL_Rect _src, int width, int height, SDL_Point offset)
	{
		name = _name;
		itemImage = texture;
		src = _src;
		imageWidth = width;
		imageHeight = height;
		imageOffset = offset;
	}

	virtual void Render() override
	{
		//cout << rect->x << " " << rect->y << " " << rect->w << " " << rect->h << endl;
		rect = { renderRect.x + imageOffset.x, renderRect.y + imageOffset.y, imageWidth, imageHeight };
		SDL_RenderCopy(Initializer.renderer, itemImage, &src, &rect);
		//itemText->SetRenderCoordinates(renderRect.x + renderRect.w / 2, renderRect.y - renderRect.h / 4);
		//itemText->Render(255);
	}
	virtual void OnMouseEnter() override
	{
		mouseIn = true;
	}
	virtual void OnMouseOut() override
	{
		mouseIn = false;
	}
};

MenuItem* MenuState::GetItemByName(MenuState* state, string name)
{
	for (auto i = state->items.begin(); i != state->items.end(); i++)
		if (dynamic_cast<MenuItemHorizontalContainer*>(*i))
		{
			auto cont = (MenuItemHorizontalContainer*)(*i);
			for (auto j = cont->items.begin(); j != cont->items.end(); j++)
				if ((*j)->name == name)
					return *j;
		}
		else if ((*i)->name == name)
			return *i;
	return nullptr;
}

class MenuHandler sealed
{
private:
	MenuHandler() {}
	~MenuHandler()
	{
		DestroyMenu();
	}
	MenuHandler(MenuHandler& mm) {}
	MenuHandler& operator = (MenuHandler& mm) {}
public:
	static MenuHandler& Instance()
	{
		static MenuHandler instance;
		return instance;
	}

	MenuState* currentState;
	vector<MenuState*> menuStates;
	MenuItem* selectedItem;
	bool enteringText;
	SDL_Texture* textureUI;
public:
	void DestroyMenu()
	{
		SDL_DestroyTexture(textureUI);
		textureUI = nullptr;

		for (int i = 0; i < menuStates.size(); i++)
		{
			menuStates[i]->RemoveState();
			delete menuStates[i];
		}
		menuStates.clear();
	}
	void Initialize()
	{
		auto surf = IMG_Load("textures/ui.png");
		textureUI = SDL_CreateTextureFromSurface(Initializer.renderer, surf);
		SDL_FreeSurface(surf);

		// MAIN MENU STATE
			SDL_Rect _rect = { 0, 0, 300, 75 };
			MenuState* state = new MenuState("main_menu", _rect);

			// main menu label
			auto label = new MenuItemLabel("label",
				new Text("fonts/pixel_font1.ttf", L"Главное меню", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }));
			state->items.push_back((MenuItem*)label);

			// new game button
			auto button = new MenuItemButton("new_game_button",
				new Text("fonts/pixel_font1.ttf", L"Новая игра", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->AddBlockingCondition(
				[&]()
				{
					int count = 0;
					for (const auto& file : directory_iterator("saves/"))
						if (file.path().extension() == ".dat")
							count++;
					return count > 7;
				});
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					SetState(GetStateByName("new_game"));
				});
			state->items.push_back((MenuItem*)button);

			// continue button
			button = new MenuItemButton("continue_button",
				new Text("fonts/pixel_font1.ttf", L"Продолжить", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->isBlocked = true;
			button->AddBlockingCondition(
				[&]()
				{
					for (const auto & file : directory_iterator("saves/"))
						if (file.path().extension() == ".dat")
							return false;
					return true;
				});
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					SetState(GetStateByName("continue_game"));
				});
			state->items.push_back((MenuItem*)button);

			// options button
			button = new MenuItemButton("high_score_button",
				new Text("fonts/pixel_font1.ttf", L"Рекорды", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					SetState(GetStateByName("high_score"));
				});
			state->items.push_back((MenuItem*)button);

			// exit game button
			button = new MenuItemButton("exit_game_button",
				new Text("fonts/pixel_font1.ttf", L"Выйти из игры", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					SetState(GetStateByName("exit"));
				});
			state->items.push_back((MenuItem*)button);

			state->Initialize();
			menuStates.push_back((MenuState*)state);

		// CONTINUE GAME STATE
			_rect = { 0, 0, 300, 75 };
			state = new MenuState("continue_game", _rect);

			// new game label
			label = new MenuItemLabel("label",
				new Text("fonts/pixel_font1.ttf", L"Продолжить игру", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }));
			state->items.push_back((MenuItem*)label);

			int saveNumber = 0;
			for (const auto & file : directory_iterator("saves/"))
				if (file.path().extension() == ".dat")
				{
					string path = file.path().stem().string().c_str();
					FILE *curSave;
					fopen_s(&curSave, ("saves/" + path + ".dat").c_str(), "rb+");
					//fseek(curSave, 0, 0);

					wstring _name = L"";
					unsigned long long _seed;
					int _day;
					int _exploredTiles;
					int _maxExploredTiles;

					// read from file

					// reading name
					int size;
					fread(&size, sizeof(int), 1, curSave);
					char* c = new char[size];
					for (int i = 0; i < size; i++)
					{
						fread(&c[i], sizeof(char), 1, curSave);
						_name.push_back(c[i]);
					}
					delete[] c;

					fread(&_seed, sizeof(unsigned long long), 1, curSave);
					fread(&_day, sizeof(int), 1, curSave);
					fread(&_exploredTiles, sizeof(int), 1, curSave);
					fread(&_maxExploredTiles, sizeof(int), 1, curSave);

					/*int curID;
					string inv = "";
					for (int i = 0; i < Inventory.slots.size(); i++)
					{
						fread(&curID, sizeof(int), 1, curSave);
						cout << curID << endl;
						if (curID > 0)
							inv.append(to_string(curID) + ";");//Inventory.AddItemToSlot(Items.GetItemByID(curID), Inventory.slots[i]);
					}*/

					fclose(curSave);

					auto container = new MenuItemHorizontalContainer("continue_container" + to_string(saveNumber), _rect);
					container->renderGapX = 100;

					// save info label
					label = new MenuItemLabel("label" + to_string(saveNumber),
						new Text("fonts/pixel_font1.ttf", _name + L", день: " + to_wstring(_day + 1), 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }));
					container->items.push_back((MenuItem*)label);

					// save continue button
					button = new MenuItemButton("save_continue_button" + to_string(saveNumber),
						new Text("fonts/pixel_font1.ttf", L"Продолжить", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
						textureUI);
					button->AddParameter("save_file_number", new int(saveNumber));
					button->AddParameter("seed", new unsigned long long(_seed));
					button->AddParameter("player_name_length", new int(size));
					button->AddParameter("player_name", new wstring(_name));
					button->AddParameter("day", new int(_day));
					button->AddParameter("explored_tiles", new int(_exploredTiles));
					button->AddParameter("max_explored_tiles", new int(_maxExploredTiles));
					//button->AddParameter("inventory", new string(inv));
					button->AddParameter("save_name", new string(path));
					button->SetClickAction(
						[&](MenuItem* caller)
						{
							auto _button = (MenuItemButton*)caller;
							FILE* _curSave;
							fopen_s(&_curSave, ("saves/" + *(string*)(_button->GetParameterByName("save_name")->value) + ".dat").c_str(), "rb+");
							//fseek(curSave, 0, 0);

							wstring _name = L"";
							unsigned long long _seed;
							int _day;
							int _exploredTiles;
							int _maxExploredTiles;

							// read from file

							// reading name
							int size;
							fread(&size, sizeof(int), 1, _curSave);
							char* c = new char[size];
							for (int i = 0; i < size; i++)
							{
								fread(&c[i], sizeof(char), 1, _curSave);
								_name.push_back(c[i]);
							}
							delete[] c;

							fread(&_seed, sizeof(unsigned long long), 1, _curSave);
							fread(&_day, sizeof(int), 1, _curSave);
							fread(&_exploredTiles, sizeof(int), 1, _curSave);
							fread(&_maxExploredTiles, sizeof(int), 1, _curSave);
							//Controller.InitializeOverlayAndInventory();
							//Controller.ReadInventoryFromFile(_curSave);

							fclose(_curSave);

							int nameLength = *(int*)(_button->GetParameterByName("player_name_length")->value);

							Controller.sessionSeed = _seed; //*(unsigned long long*)(_button->GetParameterByName("seed")->value);
							Controller.sessionDay = _day; //*(int*)(_button->GetParameterByName("day")->value);
							Controller.sessionExploredTilesCount = _exploredTiles;// *(int*)(_button->GetParameterByName("explored_tiles")->value);
							Controller.sessionMaxPossibleExploredTilesCount = _maxExploredTiles;//*(int*)(_button->GetParameterByName("max_explored_tiles")->value);
							Controller.sessionName = _name;/*.clear();
							for (int i = 0; i < nameLength; i++)
								Controller.sessionName.push_back((*(wstring*)(_button->GetParameterByName("player_name")->value))[i]);*/
							Controller.saveFileName = *(string*)(_button->GetParameterByName("save_name")->value);/*"save" +
								to_string(*(int*)(_button->GetParameterByName("save_file_number")->value));*/

							// splash screen prepare
							if (Controller.splashScreen != nullptr)
								delete Controller.splashScreen;
							Controller.splashScreen = new GameController::SplashScreen(255, 1, 1, 1, 1, 1, Controller.splashScreenText);
							Controller.splashScreenText->texts[0].text = L"День " + to_wstring(Controller.sessionDay + 1);
							Controller.splashScreenText->texts[0].RebuildText();
							Controller.splashScreenText->Initialize(
								Initializer.SCREEN_WIDTH / 2,
								Initializer.SCREEN_HEIGHT / 2 - Controller.splashScreenText->GetHeight() / 2);
							
							if (Controller.sessionDay > 0)
							{
								Controller.splashScreen->onFullTextIn =
									[&]()
								{
									Controller.StartGame(false, Controller.sessionSeed);
									FILE* curSave;
									fopen_s(&curSave, ("saves/" + Controller.saveFileName + ".dat").c_str(), "rb+");
									if (!curSave)
									{
										cout << "Save file is corrupted or does not exist." << endl;
										return;
									}
									fseek(curSave, Controller.sessionName.size() + sizeof(long long) + 4 * sizeof(int), 0);
									Controller.ReadInventoryFromFile(curSave);
									fclose(curSave);
									Controller.gameState = GameController::GameState::InGame;
								};
								Controller.splashScreen->onFullTextOut =
									[&]()
								{
									SetState(nullptr);
									Controller.gameInPause = false;
								};
							}
							else
							{
								Controller.splashScreen->onFullTextOut =
									[&]()
								{
									Controller.splashScreen->waitingDuration = 6;
									Controller.splashScreen->text = Controller.firstDaySplashText;

									Controller.splashScreen->state = GameController::SplashScreen::SplashScreenState::TextFadingIn;
									Controller.splashScreen->onFullTextIn =
										[&]()
									{
										Controller.StartGame(false, Controller.sessionSeed);
									};
									Controller.splashScreen->onFullTextOut =
										[&]()
									{
										SetState(nullptr);
										Controller.gameState = GameController::GameState::InGame;
										Controller.gameInPause = false;
									};
								};
							}
						});
					container->items.push_back((MenuItem*)button);

					// delete continue button
					/*button = new MenuItemButton("delete_continue_button" + to_string(saveNumber),
						new Text("fonts/pixel_font1.ttf", L"Удалить", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
						textureUI);
					button->AddParameter("save_file_number", new int(saveNumber));
					button->AddParameter("save_name", new string(path));
					button->AddBlockingCondition(
						[&]()
						{
							return true;
						});
					button->SetClickAction(
						[&](MenuItem* caller)
						{
							remove(("saves/" + *(string*)(caller->GetParameterByName("save_name")->value) + ".dat").c_str());
							delete MenuState::GetItemByName(currentState, "continue_container" + 
								to_string(*(int*)(caller->GetParameterByName("save_file_number")->value)));
							currentState->Initialize();
						});
					container->items.push_back((MenuItem*)button);*/

					container->Initialize();
					state->items.push_back((MenuItem*)container);
					saveNumber++;
				}

			// back button
			button = new MenuItemButton("back_button",
				new Text("fonts/pixel_font1.ttf", L"Назад", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					SetState(GetStateByName("main_menu"));
				});
			state->items.push_back((MenuItem*)button);

			state->Initialize();
			menuStates.push_back((MenuState*)state);

		// EXIT STATE
			_rect = { 0, 0, 300, 75 };
			state = new MenuState("exit", _rect);

			// exit game label
			label = new MenuItemLabel("label",
				new Text("fonts/pixel_font1.ttf", L"Вы действительно хотите выйти?", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }));
			state->items.push_back((MenuItem*)label);

			auto container = new MenuItemHorizontalContainer("exit_container", _rect);

			// yes button
			button = new MenuItemButton("yes_button",
				new Text("fonts/pixel_font1.ttf", L"Да", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					Controller.gameIsRunning = false;
					Controller.quitGame = true;
				});
			container->items.push_back((MenuItem*)button);

			// no button
			button = new MenuItemButton("no_button",
				new Text("fonts/pixel_font1.ttf", L"Нет", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					SetState(GetStateByName("main_menu"));
				});
			container->items.push_back((MenuItem*)button);

			container->Initialize();
			state->items.push_back((MenuItem*)container);

			state->Initialize();
			menuStates.push_back((MenuState*)state);

		// HIGH SCORE STATE
			_rect = { 0, 0, 300, 75 };
			state = new MenuState("high_score", _rect);

			// high score label
			label = new MenuItemLabel("label",
				new Text("fonts/pixel_font1.ttf", L"Таблица рекордов", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }));
			state->items.push_back((MenuItem*)label);

			saveNumber = 0;
			for (const auto & file : directory_iterator("saves/"))
				if (file.path().extension() == ".dat")
				{
					string path = file.path().stem().string().c_str();
					FILE *curSave;
					fopen_s(&curSave, ("saves/" + path + ".dat").c_str(), "rb+");
					//fseek(curSave, 0, 0);

					wstring _name = L"";
					unsigned long long _seed;
					int _day;
					int _exploredTiles;
					int _maxExploredTiles;

					// read from file

					// reading name
					int size;
					fread(&size, sizeof(int), 1, curSave);
					char* c = new char[size];
					for (int i = 0; i < size; i++)
					{
						fread(&c[i], sizeof(char), 1, curSave);
						_name.push_back(c[i]);
					}
					delete[] c;

					fread(&_seed, sizeof(unsigned long long), 1, curSave);
					fread(&_day, sizeof(int), 1, curSave);
					fread(&_exploredTiles, sizeof(int), 1, curSave);
					fread(&_maxExploredTiles, sizeof(int), 1, curSave);

					fclose(curSave);

					wstring l = _name + L", прожил " + to_wstring(_day + 1);
					if (to_string(_day + 1).back() == '1')
						l += L" день";
					else if (to_string(_day + 1).back() >= '2' && to_string(_day + 1).back() <= '4')
						l += L" дня";
					else l += L" дней";
					l += L", исследовал " + to_wstring(clamp(_exploredTiles * 100 * 2 / _maxExploredTiles, 0, 100)) + L"% доступных клеток";
					/*l += L" " + to_wstring(_exploredTiles);
					l += L" " + to_wstring(_maxExploredTiles);*/
					// one of high scores label
					label = new MenuItemLabel("high_score_label" + to_string(saveNumber),
						new Text("fonts/pixel_font1.ttf", l, 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }));
					state->items.push_back((MenuItem*)label);
				}

			// back button
			button = new MenuItemButton("back_button",
				new Text("fonts/pixel_font1.ttf", L"Назад", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					SetState(GetStateByName("main_menu"));
				});
			state->items.push_back((MenuItem*)button);

			state->Initialize();
			menuStates.push_back((MenuState*)state);

		// NEW GAME STATE
			_rect = { 0, 0, 300, 75 };
			state = new MenuState("new_game", _rect);

			// new game label
			label = new MenuItemLabel("label",
				new Text("fonts/pixel_font1.ttf", L"Начать новую игру", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }));
			state->items.push_back((MenuItem*)label);

			// name textbox
			auto textbox = new MenuItemTextbox("textbox",
				new Text("fonts/pixel_font1.ttf", L"введите имя", 75, 20, Text::AlignMode::ToMiddle, { 255, 190, 0 }),
				textureUI, 15);
			textbox->SetClickAction(
				[&](MenuItem* caller)
				{

				});
			state->items.push_back((MenuItem*)textbox);

			// start button
			button = new MenuItemButton("start_button",
				new Text("fonts/pixel_font1.ttf", L"Начать", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->isBlocked = true;
			button->AddBlockingCondition(
				[&]()
				{
					return ((MenuItemTextbox*)MenuState::GetItemByName(currentState, "textbox"))->isByDefault;
				});
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					Controller.sessionDay = 0;

					// splash screen prepare
					if (Controller.splashScreen != nullptr)
						delete Controller.splashScreen;
					Controller.splashScreen = new GameController::SplashScreen(255, 1, 1, 1, 1, 1, Controller.splashScreenText);
					Controller.splashScreenText->texts[0].text = L"День " + to_wstring(Controller.sessionDay + 1);
					Controller.splashScreenText->texts[0].RebuildText();
					Controller.splashScreenText->Initialize(
						Initializer.SCREEN_WIDTH / 2,
						Initializer.SCREEN_HEIGHT / 2 - Controller.splashScreenText->GetHeight() / 2);

					Controller.splashScreen->onFullTextOut =
						[&]()
					{
						Controller.splashScreen->waitingDuration = 6;
						Controller.splashScreen->text = Controller.firstDaySplashText;

						Controller.splashScreen->state = GameController::SplashScreen::SplashScreenState::TextFadingIn;
						Controller.splashScreen->onFullTextIn =
							[&]()
						{
							int savesCount = 0;
							for (const auto& file : directory_iterator("saves/"))
								if (file.path().extension() == ".dat")
									savesCount++;
							Controller.saveFileName = "save" + to_string(savesCount);
							Controller.sessionName = ((MenuItemTextbox*)MenuState::GetItemByName(currentState, "textbox"))->GetValue();
							Controller.sessionExploredTilesCount = 0;
							Controller.sessionMaxPossibleExploredTilesCount = 0;
							//Controller.InitializeOverlayAndInventory();
							Controller.StartGame(true, 0);

							// write in file
							FILE* curSave;
							fopen_s(&curSave, ("saves/save" + to_string(savesCount) + ".dat").c_str(), "wb");
							//fseek(curSave, 0, 0);
							int size = Controller.sessionName.size();
							fwrite(&size, sizeof(int), 1, curSave);
							for (int i = 0; i < size; i++)
								fwrite(&Controller.sessionName.c_str()[i], sizeof(char), 1, curSave);
							fwrite(&Controller.sessionSeed, sizeof(long long), 1, curSave);
							fwrite(&Controller.sessionDay, sizeof(int), 1, curSave);
							fwrite(&Controller.sessionExploredTilesCount, sizeof(int), 1, curSave);
							fwrite(&Controller.sessionMaxPossibleExploredTilesCount, sizeof(int), 1, curSave);
							//Controller.WriteInventoryInFile(curSave);
							fclose(curSave);

							((MenuItemTextbox*)MenuState::GetItemByName(currentState, "textbox"))->Clear();
						};
						Controller.splashScreen->onFullTextOut =
							[&]()
						{
							SetState(nullptr);
							Controller.gameState = GameController::GameState::InGame;
							Controller.gameInPause = false;
						};
					};
					//Controller.inSplashScreen = true;
					//Controller.StartGame();
					/*((MenuItemTextbox*)currentState->GetItemByName("textbox"))->Clear();
					Controller.StartGame();
					Controller.gameState = GameController::GameState::InGame;
					SetState(nullptr);*/
				});
			state->items.push_back((MenuItem*)button);

			// back button
			button = new MenuItemButton("back_button",
				new Text("fonts/pixel_font1.ttf", L"Назад", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					((MenuItemTextbox*)MenuState::GetItemByName(currentState, "textbox"))->Clear();
					SetState(GetStateByName("main_menu"));
				});
			state->items.push_back((MenuItem*)button);

			state->Initialize();
			menuStates.push_back((MenuState*)state);

		// INGAME MENU STATE
			_rect = { 0, 0, 300, 75 };
			state = new MenuState("ingame_menu", _rect);

			// resume button
			button = new MenuItemButton("resume_button",
				new Text("fonts/pixel_font1.ttf", L"Продолжить", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					SetState(nullptr);
					Controller.gameInPause = false;
				});
			state->items.push_back((MenuItem*)button);

			// main menu button
			button = new MenuItemButton("main_menu_button",
				new Text("fonts/pixel_font1.ttf", L"В главное меню", 75, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				textureUI);
			button->SetClickAction(
				[&](MenuItem* caller)
				{
					if (Controller.splashScreen != nullptr)
						delete Controller.splashScreen;
					Controller.splashScreen = new GameController::SplashScreen(255, 0.5, 0.5, 0, 0, 0, nullptr);
					Controller.splashScreen->onFullFadeIn =
						[&]()
					{
						Controller.gameIsRunning = false;
						/*Controller.gameState = GameController::GameState::InMenu;
						SetState(GetStateByName("main_menu"));*/
					};
					//Controller.inSplashScreen = true;
					//Controller.gameIsRunning = false;
				});
			state->items.push_back((MenuItem*)button);

			state->Initialize();
			menuStates.push_back((MenuState*)state);
	}
	void RenderMenu()
	{
		if (currentState != nullptr)
			currentState->Render();
	}
	void CheckMousePosition(int mouseX, int mouseY)
	{
		if (currentState != nullptr)
			for (auto i = currentState->items.begin(); i != currentState->items.end(); i++)
			{
				if (dynamic_cast<MenuItemHorizontalContainer*>(*i))
				{
					auto cont = (MenuItemHorizontalContainer*)(*i);
					for (auto j = cont->items.begin(); j != cont->items.end(); j++)
					{
						if (mouseX >= (*j)->renderRect.x &&
							mouseX <= (*j)->renderRect.x + (*j)->renderRect.w &&
							mouseY >= (*j)->renderRect.y &&
							mouseY <= (*j)->renderRect.y + (*j)->renderRect.h)
							(*j)->OnMouseEnter();
						else if ((*j)->mouseIn)
							(*j)->OnMouseOut();
					}
				}
				else
				{
					if (mouseX >= (*i)->renderRect.x &&
						mouseX <= (*i)->renderRect.x + (*i)->renderRect.w &&
						mouseY >= (*i)->renderRect.y &&
						mouseY <= (*i)->renderRect.y + (*i)->renderRect.h)
						(*i)->OnMouseEnter();
					else if ((*i)->mouseIn)
						(*i)->OnMouseOut();
				}
			}
	}
	void CheckMouseButtonDown(char mouseButton)
	{
		if (mouseButton != SDL_BUTTON_LEFT)
			return;
		if (currentState != nullptr)
		{
			auto st = currentState;
			for (auto i = st->items.begin(); i != st->items.end(); i++)
			{
				if ((*i)->mouseIn)
				{
					if (dynamic_cast<MenuItemClickable*>(*i))
					{
						selectedItem = (MenuItemClickable*)(*i);
						return;
					}
				}
				else if (dynamic_cast<MenuItemHorizontalContainer*>(*i))
				{
					auto cont = (MenuItemHorizontalContainer*)(*i);
					for (auto j = cont->items.begin(); j != cont->items.end(); j++)
						if ((*j)->mouseIn && dynamic_cast<MenuItemClickable*>(*j))
						{
							selectedItem = (MenuItemClickable*)(*j);
							return;
						}
				}
			}
			selectedItem = nullptr;
		}
	}
	void CheckMouseButtonUp(char mouseButton)
	{
		if (mouseButton != SDL_BUTTON_LEFT)
			return;
		if (currentState != nullptr)
		{
			auto st = currentState;
			for (auto i = st->items.begin(); i != st->items.end(); i++)
			{
				if ((*i)->mouseIn)
				{
					if (dynamic_cast<MenuItemClickable*>(*i))
						if ((*i) == selectedItem)
							((MenuItemClickable*)(*i))->OnClick();
				}
				else if (dynamic_cast<MenuItemHorizontalContainer*>(*i))
				{
					auto cont = (MenuItemHorizontalContainer*)(*i);
					for (auto j = cont->items.begin(); j != cont->items.end(); j++)
						if ((*j)->mouseIn && (*j) == selectedItem && dynamic_cast<MenuItemClickable*>(*j))
							((MenuItemClickable*)(*j))->OnClick();
				}
			}
			if (!dynamic_cast<MenuItemTextbox*>(selectedItem))
				selectedItem = nullptr;
		}
	}
	void ResetAllStates()
	{
		for (auto st = menuStates.begin(); st != menuStates.end(); st++)
			for (auto i = (*st)->items.begin(); i != (*st)->items.end(); i++)
				if (dynamic_cast<MenuItemHorizontalContainer*>(*i))
				{
					auto cont = (MenuItemHorizontalContainer*)(*i);
					for (auto j = cont->items.begin(); j != cont->items.end(); j++)
						(*j)->OnMouseOut();
				}
				else
					(*i)->OnMouseOut();
	}
	MenuState* GetStateByName(string stateName)
	{
		for (auto i = menuStates.begin(); i != menuStates.end(); i++)
			if ((*i)->GetStateName() == stateName)
				return *i;
		return nullptr;
	}
	void SetState(MenuState *state)
	{
		ResetAllStates();
		selectedItem = nullptr;
		currentState = state;
	}
};