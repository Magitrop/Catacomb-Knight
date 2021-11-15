#pragma once

#define Controller GameController::Instance()
#define Inventory InventoryHandler::Instance()

#include "Inventory.h"
#include "GameController.h"
#include <math.h>

using namespace std;

class ItemsDatabase final
{
private:
	ItemsDatabase() {}
	~ItemsDatabase()
	{
		for (auto i = items.begin(); i != items.end(); i++)
			delete *i;
	}
	ItemsDatabase(ItemsDatabase& mm) {}
	ItemsDatabase& operator = (ItemsDatabase& mm) {}

	vector<Item*> items;
public:
	static ItemsDatabase& Instance()
	{
		static ItemsDatabase instance;
		return instance;
	}

	const SDL_Color standart = { 100, 100, 100 };
	const SDL_Color common = { 22, 218, 94 };
	const SDL_Color rare = { 59, 70, 226 };
	const SDL_Color epic = { 121, 14, 203 };
	const SDL_Color legendary = { 229, 174, 45 };
	const SDL_Color relic = { 255, 245, 146 };

	void Initialize()
	{
		for (auto i = items.begin(); i != items.end(); i++)
			delete *i;
		items.clear();

		Item* item = new Item(1,
			"small_healing_potion",
			L"Малое зелье лечения",
			Initializer.tilesetFloor,
			{
				2 * Initializer.TEXTURE_RES,
				4 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = true;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 400;
		};
		item->OnItemUse =
			[&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				Creature::Heal((int)(_sender->maxHealth * 0.2), _sender, nullptr);
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Малое зелье лечения", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"20% здоровья", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(2,
			"medium_healing_potion",
			L"Среднее зелье лечения",
			Initializer.tilesetFloor,
			{
				1 * Initializer.TEXTURE_RES,
				4 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = true;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 270;
		};
		item->OnItemUse =
			[&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				Creature::Heal((int)(_sender->maxHealth * 0.4), (Creature*)sender, nullptr);
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Среднее зелье лечения", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"40% здоровья", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(3,
			"big_healing_potion",
			L"Большое зелье лечения",
			Initializer.tilesetFloor,
			{
				0,
				4 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = true;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 120;
		};
		item->OnItemUse =
			[&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				Creature::Heal((int)(_sender->maxHealth * 0.75), (Creature*)sender, nullptr);
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Большое зелье лечения", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"75% здоровья", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(4,
			"blue_key",
			L"Синий ключ",
			Initializer.tilesetFloor,
			{
				0,
				3 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = false;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(8,
				new Text[8]
				{
					Text("fonts/pixel_font1.ttf", L"Синий ключ", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Позволяет открыть синюю дверь", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(5,
			"green_key",
			L"Зеленый ключ",
			Initializer.tilesetFloor,
			{
				1 * Initializer.TEXTURE_RES,
				3 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = false;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(8,
				new Text[8]
				{
					Text("fonts/pixel_font1.ttf", L"Зеленый ключ", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Позволяет открыть зеленую дверь", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(6,
			"red_key",
			L"Красный ключ",
			Initializer.tilesetFloor,
			{
				2 * Initializer.TEXTURE_RES,
				3 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = false;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(8,
				new Text[8]
				{
					Text("fonts/pixel_font1.ttf", L"Красный ключ", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Позволяет открыть красную дверь", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(7,
			"purple_key",
			L"Фиолетовый ключ",
			Initializer.tilesetFloor,
			{
				3 * Initializer.TEXTURE_RES,
				3 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = false;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(8,
				new Text[8]
				{
					Text("fonts/pixel_font1.ttf", L"Фиолетовый ключ", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Позволяет открыть фиолетовую дверь", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(8,
			"yellow_key",
			L"Желтый ключ",
			Initializer.tilesetFloor,
			{
				4 * Initializer.TEXTURE_RES,
				3 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = false;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(8,
				new Text[8]
				{
					Text("fonts/pixel_font1.ttf", L"Желтый ключ", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Позволяет открыть желтую дверь", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(9,
			"small_dagger",
			L"Небольшой кинжал",
			Initializer.tilesetFloor,
			{
				3 * Initializer.TEXTURE_RES,
				4 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 30 + 10 * Controller.sessionDay;
			result = clamp(result, 0, 60);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 11 + Controller.sessionDay * 6;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 10 + Controller.sessionDay * 6);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Небольшой кинжал", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(10 + Controller.sessionDay * 6) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(10,
			"steel_sword",
			L"Стальной меч",
			Initializer.tilesetFloor,
			{
				4 * Initializer.TEXTURE_RES,
				4 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = common;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 14 + 13 * Controller.sessionDay;
			result = clamp(result, 0, 70);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 18 + Controller.sessionDay * 9;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 18 + Controller.sessionDay * 9);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Стальной меч", 50, 0, Text::AlignMode::ToMiddle, common),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(18 + Controller.sessionDay * 9) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(11,
			"war_axe",
			L"Боевой топор",
			Initializer.tilesetFloor,
			{
				3 * Initializer.TEXTURE_RES,
				5 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = common;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 9 + 2 * Controller.sessionDay;
			result = clamp(result, 0, 20);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 25 + Controller.sessionDay * 13;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 25 + Controller.sessionDay * 13);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			auto _receiver = dynamic_cast<Creature*>(receiver);
			if (_sender != nullptr && _receiver != nullptr && rand() % 100 < 15)
				Creature::DealDamage(25 + Controller.sessionDay * 13, _receiver, _sender);
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Боевой топор", 50, 0, Text::AlignMode::ToMiddle, common),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(25 + Controller.sessionDay * 13) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"С вероятностью 15%", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"наносит дополнительно " + to_wstring(25 + Controller.sessionDay * 13) + L" ед. урона", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(12,
			"steel_spear",
			L"Стальное копье",
			Initializer.tilesetFloor,
			{
				4 * Initializer.TEXTURE_RES,
				5 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = common;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 4 + 3 * Controller.sessionDay;
			result = clamp(result, 0, 20);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 26 + Controller.sessionDay * 15;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 26 + Controller.sessionDay * 15);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			auto _receiver = dynamic_cast<Creature*>(receiver);
			if (_sender != nullptr && _receiver != nullptr)
			{
				Tile* t = Controller.map->tilemap->GetTile(_receiver->x, _receiver->y);
				auto _back = dynamic_cast<Creature*>(Controller.map->tilemap->GetTileObjectInDirection(t, _sender));
				if (_back != nullptr && _back != _sender)
					Creature::DealDamage(26 + Controller.sessionDay * 15, _back, _sender);
			}
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(11,
				new Text[11]
				{
					Text("fonts/pixel_font1.ttf", L"Стальное копье", 50, 0, Text::AlignMode::ToMiddle, common),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(26 + Controller.sessionDay * 15) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Наносит также " + to_wstring(26 + Controller.sessionDay * 15) + L" ед. урона", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"существу, находящемуся", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"позади атакуемого существа", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"в направлении удара", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(13,
			"bloodcall_mace",
			L"Булава Зова крови",
			Initializer.tilesetFloor,
			{
				3 * Initializer.TEXTURE_RES,
				7 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = rare;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 18 + Controller.sessionDay * 12;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 18 + Controller.sessionDay * 12);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			auto _receiver = dynamic_cast<Creature*>(receiver);
			if (_sender != nullptr && _receiver != nullptr)
			{
				Controller.visualEffects.push_back(new VisualEffect
				(
					new Animation(0.075, 5,
						{
							new AnimationFrame(16, 16, 0 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 1 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 3 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 4 * 16, 0, 0.5, 0.5),
						}),
						"blood",
						"textures/blood_visual_effect.png",
					5, _receiver->x, _receiver->y
				));
				Creature::Heal(4 + Controller.sessionDay * 4, _sender, nullptr);
			}
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(10,
				new Text[10]
				{
					Text("fonts/pixel_font1.ttf", L"Булава Зова крови", 50, 0, Text::AlignMode::ToMiddle, rare),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(18 + Controller.sessionDay * 12) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"После каждого успешного удара", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", to_wstring(4 + Controller.sessionDay * 4) + L" ед. здоровья", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(14,
			"amethyst_scythe",
			L"Аметистовая коса",
			Initializer.tilesetFloor,
			{
				4 * Initializer.TEXTURE_RES,
				6 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = epic;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 25 + Controller.sessionDay * 12;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 25 + Controller.sessionDay * 12);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				Tile* t;
				Creature* target;
				switch (_sender->facingDirection)
				{
				case Up:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x - 1, _sender->y - 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y - 1);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.075, 7,
							{
								new AnimationFrame(48, 16, 3 * 16, 3 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 3 * 16, 4 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 16, 3 * 16, 5 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 3 * 16, 6 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 3 * 16, 7 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 3 * 16, 8 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 16, 3 * 16, 9 * 16, 0.5, 0.5)
							}),
						"amethyst_scythe",
						"textures/amethyst_scythe_visual_effect.png",
						7, _sender->x, _sender->y - 1
					));
					break;
				}
				case Down:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x - 1, _sender->y + 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y + 1);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.075, 7,
							{
								new AnimationFrame(48, 16, 0 * 16, 9 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 0 * 16, 8 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 16, 0 * 16, 7 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 0 * 16, 6 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 0 * 16, 5 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 0 * 16, 4 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 16, 0 * 16, 3 * 16, 0.5, 0.5)
							}),
						"amethyst_scythe",
						"textures/amethyst_scythe_visual_effect.png",
						7, _sender->x, _sender->y + 1
						));
					break;
				}
				case Right:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y - 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y + 1);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.075, 7,
							{
								new AnimationFrame(16, 48, 6 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 5 * 16, 0 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(16, 48, 4 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 3 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 2 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 1 * 16, 0 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(16, 48, 0 * 16, 0 * 16, 0.5, 0.5)
							}),
						"amethyst_scythe",
						"textures/amethyst_scythe_visual_effect.png",
						7, _sender->x + 1, _sender->y
						));
					break;
				}
				case Left:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x - 1, _sender->y - 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x - 1, _sender->y + 1);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.075, 7,
							{
								new AnimationFrame(16, 48, 7 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 8 * 16, 0 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(16, 48, 9 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 10 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 11 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 12 * 16, 0 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(16, 48, 13 * 16, 0 * 16, 0.5, 0.5)
							}),
						"amethyst_scythe",
						"textures/amethyst_scythe_visual_effect.png",
						7, _sender->x - 1, _sender->y
						));
					break;
				}
				default: break;
				}
			}
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(11,
				new Text[11]
				{
					Text("fonts/pixel_font1.ttf", L"Аметистовая коса", 50, 0, Text::AlignMode::ToMiddle, epic),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(25 + Controller.sessionDay * 12) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Наносит также " + to_wstring(25 + Controller.sessionDay * 12) + L" ед. урона", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"существам, находящимся", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"на клетках по диагонали", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"от героя в направлении удара", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(15,
			"astral_greatsword",
			L"Астральный Великий меч",
			Initializer.tilesetFloor,
			{
				5 * Initializer.TEXTURE_RES,
				6 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = epic;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 100 + Controller.sessionDay * 50;
				//_sender->maxActionsCount -= 2;
				if (_sender->actionsLeft > _sender->maxActionsCount)
					_sender->actionsLeft = _sender->maxActionsCount;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 100 + Controller.sessionDay * 50);
				//_sender->maxActionsCount += 2;
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				Tile* t;
				Creature* target;
				switch (_sender->facingDirection)
				{
				case Up:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x, _sender->y - 2);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x, _sender->y - 3);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.1, 7,
							{
								new AnimationFrame(48, 48, 0 * 48, 2 * 48, 1, 0.166),
								new AnimationFrame(48, 48, 1 * 48, 2 * 48, 1, 0.333),
								new AnimationFrame(48, 48, 2 * 48, 2 * 48, 1, 1.166),
								new AnimationFrame(48, 48, 3 * 48, 2 * 48, 0.833, 1.166,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(100 + Controller.sessionDay * 50, _target, (Creature*)(_parameters[2]));
										}

										t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(100 + Controller.sessionDay * 50, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 48, 4 * 48, 2 * 48, 0.833, 1.166),
								new AnimationFrame(48, 48, 5 * 48, 2 * 48, 0.833, 1.166),
								new AnimationFrame(48, 48, 6 * 48, 2 * 48, 0.833, 1.166)
							}),
						"astral_greatsword",
						"textures/astral_greatsword_visual_effect.png",
						7, _sender->x, _sender->y
						));
					break;
				}
				case Down:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x, _sender->y + 2);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x, _sender->y + 3);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.1, 7,
							{
								new AnimationFrame(48, 48, 6 * 48, 3 * 48, 0, 0.833),
								new AnimationFrame(48, 48, 5 * 48, 3 * 48, 0, 0.833),
								new AnimationFrame(48, 48, 4 * 48, 3 * 48, 0, -0.166),
								new AnimationFrame(48, 48, 3 * 48, 3 * 48, 0.166, -0.166,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(100 + Controller.sessionDay * 50, _target, (Creature*)(_parameters[2]));
										}

										t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(100 + Controller.sessionDay * 50, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 48, 2 * 48, 3 * 48, 0.166, -0.166),
								new AnimationFrame(48, 48, 1 * 48, 3 * 48, 0.166, -0.166),
								new AnimationFrame(48, 48, 0 * 48, 3 * 48, 0.166, -0.166)
							}),
						"astral_greatsword",
						"textures/astral_greatsword_visual_effect.png",
						7, _sender->x, _sender->y
					));
					break;
				}
				case Right:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x + 2, _sender->y);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x + 3, _sender->y);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.1, 7,
							{
								new AnimationFrame(48, 48, 0 * 48, 0, 0.833, 1),
								new AnimationFrame(48, 48, 1 * 48, 0, 0.666, 1),
								new AnimationFrame(48, 48, 2 * 48, 0, -0.166, 1),
								new AnimationFrame(48, 48, 3 * 48, 0, -0.166, 0.833,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(100 + Controller.sessionDay * 50, _target, (Creature*)(_parameters[2]));
										}

										t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(100 + Controller.sessionDay * 50, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 48, 4 * 48, 0, -0.166, 0.833),
								new AnimationFrame(48, 48, 5 * 48, 0, -0.166, 0.833),
								new AnimationFrame(48, 48, 6 * 48, 0, -0.166, 0.833)
							}),
						"astral_greatsword",
						"textures/astral_greatsword_visual_effect.png",
						7, _sender->x, _sender->y
					));
					break;
				}
				case Left:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x - 2, _sender->y);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x - 3, _sender->y);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.1, 7,
							{
								new AnimationFrame(48, 48, 6 * 48, 48, 0.166, 1),
								new AnimationFrame(48, 48, 5 * 48, 48, 0.333, 1),
								new AnimationFrame(48, 48, 4 * 48, 48, 1.166, 1),
								new AnimationFrame(48, 48, 3 * 48, 48, 1.166, 0.833,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(100 + Controller.sessionDay * 50, _target, (Creature*)(_parameters[2]));
										}

										t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[2]))
												Creature::DealDamage(100 + Controller.sessionDay * 50, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 48, 2 * 48, 48, 1.166, 0.833),
								new AnimationFrame(48, 48, 1 * 48, 48, 1.166, 0.833),
								new AnimationFrame(48, 48, 0 * 48, 48, 1.166, 0.833)
							}),
						"astral_greatsword",
						"textures/astral_greatsword_visual_effect.png",
						7, _sender->x, _sender->y
					));
					break;
				}
				default: break;
				}
			}
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(14,
				new Text[14]
				{
					Text("fonts/pixel_font1.ttf", L"Астральный Великий меч", 50, 0, Text::AlignMode::ToMiddle, epic),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(100 + Controller.sessionDay * 50) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Наносит также " + to_wstring(100 + Controller.sessionDay * 50) + L" ед. урона", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"существам, находящимся", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"через одну и через две клетки", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"от героя в направлении удара", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходует 2 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при атаке", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(16,
			"lifestone_necklace",
			L"Ожерелье с Камнем жизни",
			Initializer.tilesetFloor,
			{
				6 * Initializer.TEXTURE_RES,
				4 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = rare;
		item->isConsumable = false;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) 
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->maxHealth += 65 + Controller.sessionDay * 18;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemTrash = [&](TileObject* sender) 
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->maxHealth -= 65 + Controller.sessionDay * 18;
				if (_sender->curHealth > _sender->maxHealth)
					_sender->curHealth = MoveTowards(_sender->curHealth, 1, 65 + Controller.sessionDay * 18);
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Ожерелье с Камнем жизни", 50, 0, Text::AlignMode::ToMiddle, common),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Дарует дополнительные", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", to_wstring(65 + Controller.sessionDay * 18) + L" ед. здоровья владельцу", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не требуется экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(17,
			"lifestone_pendant",
			L"Подвеска с Камнем жизни",
			Initializer.tilesetFloor,
			{
				7 * Initializer.TEXTURE_RES,
				4 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = rare;
		item->isConsumable = false;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->maxHealth += 40 + Controller.sessionDay * 15;
				_sender->viewDistance += 1;
				_sender->sightDistance += 1;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemTrash = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->maxHealth -= 40 + Controller.sessionDay * 15;
				if (_sender->curHealth > _sender->maxHealth)
					_sender->curHealth = MoveTowards(_sender->curHealth, 1, 40 + Controller.sessionDay * 15);
				_sender->viewDistance = MoveTowards(_sender->viewDistance, 2, 1);
				_sender->sightDistance = MoveTowards(_sender->sightDistance, 2, 1);
				MapHandler::Instance().RecalculateFogOfWar(Controller.map,
					Controller.map->tilemap->GetTile(Controller.mainHero->x, Controller.mainHero->y),
					Controller.mainHero->viewDistance);
				MapHandler::Instance().RecalculateVisibleLands(Controller.map,
					Controller.map->tilemap->GetTile(Controller.mainHero->x, Controller.mainHero->y),
					Controller.mainHero->sightDistance);
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(8,
				new Text[8]
				{
					Text("fonts/pixel_font1.ttf", L"Подвеска с Камнем жизни", 50, 0, Text::AlignMode::ToMiddle, rare),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Дарует владельцу дополнительные", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", to_wstring(40 + Controller.sessionDay * 15) + L" ед. здоровья", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"и 1 ед. к дальности обзора", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"и раскрытия тумана войны", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не требуется экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(18,
			"sunlight_shield",
			L"Щит Солнечного света",
			Initializer.tilesetFloor,
			{
				8 * Initializer.TEXTURE_RES,
				4 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = rare;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->visualEffects.push_back(new VisualEffect
				(
					new Animation(0.25, 4,
						{
							new AnimationFrame(16, 16, 0 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 1 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 3 * 16, 0, 0.5, 0.5),
						}),
						"sunlight_shield",
						"textures/sunlight_shield_visual_effect.png",
						4, _sender->x, _sender->y
						));
				_sender->damage += 10 + Controller.sessionDay * 5;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				for (auto i = _sender->visualEffects.begin(); i != _sender->visualEffects.end(); i++)
					if ((*i)->effectName == "sunlight_shield")
					{
						delete (*i);
						_sender->visualEffects.erase(i);
						break;
					}
				_sender->damage = MoveTowards(_sender->damage, 1, 10 + Controller.sessionDay * 5);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) 
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr && rand() % 100 <= 33)
				_sender->actionsLeft = MoveTowards(_sender->actionsLeft, _sender->maxActionsCount, 1);
		};
		item->OnGettingDamageWith = [&](TileObject* sender, int amount, TileObject* from, int* totalAmountChange) 
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				*totalAmountChange += 25 + Controller.sessionDay * 12;
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Щит Солнечного света", 50, 0, Text::AlignMode::ToMiddle, rare),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(10 + Controller.sessionDay * 5) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Защищает владельца от", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", to_wstring(25 + Controller.sessionDay * 12) + L" ед. урона", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(19,
			"soulburning_axe",
			L"Выжигающий душу",
			Initializer.tilesetFloor,
			{
				5 * Initializer.TEXTURE_RES,
				7 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = epic;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 35 + Controller.sessionDay * 15;
				_sender->visualEffects.push_back(new VisualEffect
				(
					new Animation(0.25, 4,
						{
							new AnimationFrame(16, 16, 0 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 1 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
							new AnimationFrame(16, 16, 3 * 16, 0, 0.5, 0.5),
						}),
						"soulburning_axe",
						"textures/soulburning_axe_visual_effect.png",
						4, _sender->x, _sender->y
						));
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 35 + Controller.sessionDay * 15);
				for (auto i = _sender->visualEffects.begin(); i != _sender->visualEffects.end(); i++)
					if ((*i)->effectName == "soulburning_axe")
					{
						delete (*i);
						_sender->visualEffects.erase(i);
						break;
					}
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			auto _receiver = dynamic_cast<Creature*>(receiver);
			if (_sender != nullptr && _receiver != nullptr)
			{
				int lostHealthPercent = 100 - (_sender->curHealth * 100) / _sender->maxHealth;
				if (lostHealthPercent >= 60)
				{
					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.15, 4,
							{
								new AnimationFrame(16, 16, 0 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 1 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 3 * 16, 0, 0.5, 0.5)
							}),
							"soulburn",
							"textures/soulburn_visual_effect.png",
							4, _receiver->x, _receiver->y
							));
					Creature::DealDamage(35 + Controller.sessionDay * 15, _receiver, _sender);
				}
			}
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(11,
				new Text[11]
				{
					Text("fonts/pixel_font1.ttf", L"Выжигающий душу", 50, 0, Text::AlignMode::ToMiddle, epic),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(35 + Controller.sessionDay * 15) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Наносит дополнительно", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", to_wstring(35 + Controller.sessionDay * 15) + L" ед. урона, если", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"у владельца не более", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"40% здоровья", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(20,
			"small_energy_potion",
			L"Малое зелье энергии",
			Initializer.tilesetFloor,
			{
				2 * Initializer.TEXTURE_RES,
				5 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = true;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 250;
		};
		item->OnItemUse =
			[&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				_sender->actionsLeft = MoveTowards(_sender->actionsLeft, _sender->maxActionsCount, 1);
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Малое зелье энергии", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"1 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(21,
			"medium_energy_potion",
			L"Среднее зелье энергии",
			Initializer.tilesetFloor,
			{
				1 * Initializer.TEXTURE_RES,
				5 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = true;
		item->isEquippable = false;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 120;
		};
		item->OnItemUse =
			[&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				_sender->actionsLeft = MoveTowards(_sender->actionsLeft, _sender->maxActionsCount, 2);
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Среднее зелье энергии", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"2 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(22,
			"big_energy_potion",
			L"Большое зелье энергии",
			Initializer.tilesetFloor,
			{
				0,
				5 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = true;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 60;
		};
		item->OnItemUse =
			[&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				_sender->actionsLeft = MoveTowards(_sender->actionsLeft, _sender->maxActionsCount, 3);
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Большое зелье энергии", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"3 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(23,
			"chicken_leg",
			L"Куриная ножка",
			Initializer.tilesetFloor,
			{
				0,
				6 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = true;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 60;
		};
		item->OnItemUse =
			[&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				Creature::Heal((int)(_sender->maxHealth * 0.2), (Creature*)sender, nullptr);
				_sender->actionsLeft = MoveTowards(_sender->actionsLeft, _sender->maxActionsCount, 2);
			}
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Куриная ножка", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"20% здоровья и 2 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(24,
			"fresh_apple",
			L"Свежее яблоко",
			Initializer.tilesetFloor,
			{
				1 * Initializer.TEXTURE_RES,
				6 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = true;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 60;
		};
		item->OnItemUse =
			[&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				Creature::Heal((int)(_sender->maxHealth * 0.4), (Creature*)sender, nullptr);
				_sender->actionsLeft = MoveTowards(_sender->actionsLeft, _sender->maxActionsCount, 1);
			}
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(9,
				new Text[9]
				{
					Text("fonts/pixel_font1.ttf", L"Свежее яблоко", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Восстанавливает владельцу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"40% здоровья и 1 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(25,
			"jar_lightning",
			L"Молния в банке",
			Initializer.tilesetFloor,
			{
				2 * Initializer.TEXTURE_RES,
				6 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = common;
		item->usingRange = 5;
		item->isConsumable = true;
		item->isEquippable = false;
		item->canUseInFogOfWar = false;
		item->requiresTile = true;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 60;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemUseAt = [&](TileObject* sender, Tile* at)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr && at != nullptr)
			{
				vector<void*> params;
				params.push_back(at);
				params.push_back(_sender);
				Controller.visualEffects.push_back(new VisualEffect
				(
					new Animation(0.0625, 10,
						{
							new AnimationFrame(16, 32, 0 * 16, 0, 0.5, 0.75),
							new AnimationFrame(16, 32, 1 * 16, 0, 0.5, 0.75),
							new AnimationFrame(16, 32, 2 * 16, 0, 0.5, 0.75),
							new AnimationFrame(16, 32, 3 * 16, 0, 0.5, 0.75,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr)
												Creature::DealDamage(50 + Controller.sessionDay * 25, _target, nullptr/*(Creature*)(_parameters[1])*/);
										}
									}, params),
							new AnimationFrame(16, 32, 4 * 16, 0, 0.5, 0.75),
							new AnimationFrame(16, 32, 5 * 16, 0, 0.5, 0.75),
							new AnimationFrame(16, 32, 6 * 16, 0, 0.5, 0.75),
							new AnimationFrame(16, 32, 7 * 16, 0, 0.5, 0.75),
							new AnimationFrame(16, 32, 8 * 16, 0, 0.5, 0.75),
							new AnimationFrame(16, 32, 9 * 16, 0, 0.5, 0.75)
						}),
					"lightning_bolt",
					"textures/lightning_bolt_visual_effect.png",
					10, at->x, at->y
				));
			}
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(10,
				new Text[10]
				{
					Text("fonts/pixel_font1.ttf", L"Молния в банке", 50, 0, Text::AlignMode::ToMiddle, common),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Выпускает молнию,", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"наносящую " + to_wstring(50 + Controller.sessionDay * 25) + L" урона существу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на выбранной клетке", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не расходует энергию", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходуемый предмет", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(26,
			"wooden_stick",
			L"Деревянная палка",
			Initializer.tilesetFloor,
			{
				3 * Initializer.TEXTURE_RES,
				8 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 40 + 10 * Controller.sessionDay;
			result = clamp(result, 0, 70);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 8 + Controller.sessionDay * 5;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 8 + Controller.sessionDay * 5);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Деревянная палка", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(8 + Controller.sessionDay * 5) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(27,
			"wooden_club",
			L"Деревянная дубина",
			Initializer.tilesetFloor,
			{
				4 * Initializer.TEXTURE_RES,
				8 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 30 + 9 * Controller.sessionDay;
			result = clamp(result, 0, 60);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 14 + Controller.sessionDay * 8;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 14 + Controller.sessionDay * 8);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Деревянная дубина", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Увеличивает урон владельца", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на " + to_wstring(14 + Controller.sessionDay * 8) + L" ед.", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255}),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(28,
			"wooden_shield",
			L"Деревянный щит",
			Initializer.tilesetFloor,
			{
				6 * Initializer.TEXTURE_RES,
				5 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 40 + 10 * Controller.sessionDay;
			result = clamp(result, 0, 70);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->OnGettingDamageWith = [&](TileObject* sender, int amount, TileObject* from, int* totalAmountChange)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				*totalAmountChange += 3 + Controller.sessionDay * 3;
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Деревянный щит", 50, 0, Text::AlignMode::ToMiddle, standart),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Защищает владельца от", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", to_wstring(3 + Controller.sessionDay * 3) + L" ед. урона", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не требуется экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(29,
			"steel_shield",
			L"Стальной щит",
			Initializer.tilesetFloor,
			{
				7 * Initializer.TEXTURE_RES,
				5 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = common;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 20 + 8 * Controller.sessionDay;
			result = clamp(result, 0, 60);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->OnGettingDamageWith = [&](TileObject* sender, int amount, TileObject* from, int* totalAmountChange)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				*totalAmountChange += 6 + Controller.sessionDay * 4;
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Стальной щит", 50, 0, Text::AlignMode::ToMiddle, common),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Защищает владельца от", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", to_wstring(6 + Controller.sessionDay * 4) + L" ед. урона", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не требуется экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(30,
			"amethyst_shield",
			L"Аметистовый щит",
			Initializer.tilesetFloor,
			{
				8 * Initializer.TEXTURE_RES,
				5 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = rare;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 20 + 8 * Controller.sessionDay;
			result = clamp(result, 0, 60);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->OnGettingDamageWith = [&](TileObject* sender, int amount, TileObject* from, int* totalAmountChange)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
				*totalAmountChange += 13 + Controller.sessionDay * 8;
		};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Аметистовый щит", 50, 0, Text::AlignMode::ToMiddle, rare),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Защищает владельца от", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", to_wstring(13 + Controller.sessionDay * 8) + L" ед. урона", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не требуется экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(31,
			"lightning_rod",
			L"Жезл молний",
			Initializer.tilesetFloor,
			{
				5 * Initializer.TEXTURE_RES,
				8 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = rare;
		item->usingRange = 5;
		item->isConsumable = false;
		item->isEquippable = true;
		item->canUseInFogOfWar = false;
		item->requiresTile = true;
		item->worksOnlyIfEquipped = true;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			int result = 2 + 1 * Controller.sessionDay;
			result = clamp(result, 0, 15);
			return result;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemUseAt = [&](TileObject* sender, Tile* at)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr && at != nullptr)
			{
				if (_sender->actionsLeft > 0 || Controller.godMode)
				{
					if (!Controller.godMode)
						_sender->actionsLeft -= 1;
					vector<void*> params;
					params.push_back(at);
					params.push_back(_sender);
					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.0625, 10,
							{
								new AnimationFrame(16, 32, 0 * 16, 0, 0.5, 0.75),
								new AnimationFrame(16, 32, 1 * 16, 0, 0.5, 0.75),
								new AnimationFrame(16, 32, 2 * 16, 0, 0.5, 0.75,
										[=](vector<void*> _parameters)
										{
											Creature* _target;
											Tile* t = (Tile*)(_parameters[0]);
											if (t != nullptr)
											{
												_target = dynamic_cast<Creature*>(t->GetTileObject());
												if (_target != nullptr)
													Creature::DealDamage(40 + Controller.sessionDay * 20, _target, nullptr/*(Creature*)(_parameters[1])*/);
											}
										}, params),
								new AnimationFrame(16, 32, 3 * 16, 0, 0.5, 0.75),
								new AnimationFrame(16, 32, 4 * 16, 0, 0.5, 0.75),
								new AnimationFrame(16, 32, 5 * 16, 0, 0.5, 0.75),
								new AnimationFrame(16, 32, 6 * 16, 0, 0.5, 0.75),
								new AnimationFrame(16, 32, 7 * 16, 0, 0.5, 0.75),
								new AnimationFrame(16, 32, 8 * 16, 0, 0.5, 0.75),
								new AnimationFrame(16, 32, 9 * 16, 0, 0.5, 0.75)
							}),
						"lightning_bolt",
						"textures/lightning_bolt_visual_effect.png",
						10, at->x, at->y
						));
				}
			}
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(10,
				new Text[10]
				{
					Text("fonts/pixel_font1.ttf", L"Жезл молний", 50, 0, Text::AlignMode::ToMiddle, rare),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Призывает молнию,", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"наносящую " + to_wstring(50 + Controller.sessionDay * 25) + L" урона существу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на выбранной клетке", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходует 1 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(32,
			"lesser_astral_rod",
			L"Малый Астральный жезл",
			Initializer.tilesetFloor,
			{
				3 * Initializer.TEXTURE_RES,
				9 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = rare;
		item->usingRange = 5;
		item->isConsumable = false;
		item->isEquippable = true;
		item->canUseInFogOfWar = false;
		item->requiresTile = true;
		item->worksOnlyIfEquipped = true;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemUseAt = [&](TileObject* sender, Tile* at)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr && at != nullptr)
			{
				if (_sender->actionsLeft > 0 || Controller.godMode)
				{
					if (!Controller.godMode)
						_sender->actionsLeft -= 1;
					vector<void*> params;
					params.push_back(at);
					params.push_back(_sender);
					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.135, 19,
							{
								new AnimationFrame(16, 16, 0 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 1 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 3 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 4 * 16, 0, 0.5, 0.5,
										[=](vector<void*> _parameters)
										{
											Creature* _target;
											Tile* t = (Tile*)(_parameters[0]);
											if (t != nullptr)
											{
												_target = dynamic_cast<Creature*>(t->GetTileObject());
												if (_target != nullptr)
													Creature::DealDamage(70 + Controller.sessionDay * 30, _target, nullptr/*(Creature*)(_parameters[1])*/);
											}
										}, params),
								new AnimationFrame(16, 16, 5 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 6 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 7 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 8 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 2 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 9 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 10 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 11 * 16, 0, 0.5, 0.5),
								new AnimationFrame(16, 16, 12 * 16, 0, 0.5, 0.5),
							}),
						"astral_rift",
						"textures/astral_rift_visual_effect.png",
						19, at->x, at->y, true
						));
				}
			}
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(10,
				new Text[10]
				{
					Text("fonts/pixel_font1.ttf", L"Малый Астральный жезл", 50, 0, Text::AlignMode::ToMiddle, rare),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Создает небольшой Астральный разлом,", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"наносящий " + to_wstring(70 + Controller.sessionDay * 30) + L" урона существу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на выбранной клетке", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходует 1 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(33,
			"starlight",
			L"Звездный Свет",
			Initializer.tilesetFloor,
			{
				4 * Initializer.TEXTURE_RES,
				9 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = legendary;
		item->usingRange = 6;
		item->isConsumable = false;
		item->isEquippable = true;
		item->canUseInFogOfWar = false;
		item->requiresTile = true;
		item->worksOnlyIfEquipped = true;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemUseAt = [&](TileObject* sender, Tile* at)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr && at != nullptr)
			{
				if (_sender->actionsLeft >= 2 || Controller.godMode)
				{
					if (!Controller.godMode)
						_sender->actionsLeft -= 2;
					vector<void*> params;
					params.push_back(at);
					params.push_back(_sender);
					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.1, 17,
							{
								new AnimationFrame(16, 64, 0 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(16, 64, 1 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(16, 64, 3 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(16, 64, 4 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(32, 64, 5 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(32, 64, 7 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(32, 64, 9 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(32, 64, 11 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(32, 64, 13 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(32, 64, 15 * 16, 0, 0.5, 0.9375,
										[=](vector<void*> _parameters)
										{
											Creature* _target;
											Tile* t = (Tile*)(_parameters[0]);
											if (t != nullptr)
											{
												_target = dynamic_cast<Creature*>(t->GetTileObject());
												if (_target != nullptr)
													Creature::DealDamage(160 + Controller.sessionDay * 70, _target, nullptr/*(Creature*)(_parameters[1])*/);
											}
										}, params),
								new AnimationFrame(32, 64, 17 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(32, 64, 19 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(16, 64, 23 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(16, 64, 24 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(16, 64, 25 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(16, 64, 26 * 16, 0, 0.5, 0.9375),
								new AnimationFrame(16, 64, 27 * 16, 0, 0.5, 0.9375),
							}),
						"starlight_eruption",
						"textures/starlight_eruption_visual_effect.png",
						17, at->x, at->y
						));
				}
			}
		};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer* &itemDescription) 
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(10,
				new Text[10]
				{
					Text("fonts/pixel_font1.ttf", L"Звездный свет", 50, 0, Text::AlignMode::ToMiddle, legendary),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Создает луч космической энергии,", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"наносящий " + to_wstring(160 + Controller.sessionDay * 70) + L" урона существу", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"на выбранной клетке", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Расходует 2 ед. энергии", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"при использовании", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Необходимо экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(34,
			"lifestone_ring",
			L"Кольцо с Камнем жизни",
			Initializer.tilesetFloor,
			{
				6 * Initializer.TEXTURE_RES,
				6 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = common;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->maxHealth += 25 + Controller.sessionDay * 12;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemTrash = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->maxHealth -= 25 + Controller.sessionDay * 12;
				if (_sender->curHealth > _sender->maxHealth)
					_sender->curHealth = MoveTowards(_sender->curHealth, 1, 25 + Controller.sessionDay * 12);
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemEquip = [&](TileObject* sender) {};
		item->OnItemUnequip = [&](TileObject* sender) {};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver) {};
		item->RebuildDescriptionText = [&](TileObject* sender, TextsContainer*& itemDescription)
		{
			if (itemDescription != nullptr)
				delete[] itemDescription->texts;
			delete itemDescription;
			itemDescription = new TextsContainer(6,
				new Text[6]
				{
					Text("fonts/pixel_font1.ttf", L"Кольцо с Камнем жизни", 50, 0, Text::AlignMode::ToMiddle, common),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Дарует дополнительные", 50, 0, Text::AlignMode::ToMiddle, {255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", to_wstring(25 + Controller.sessionDay * 12) + L" ед. здоровья владельцу", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
					Text("fonts/pixel_font1.ttf", L"Не требуется экипировать", 50, 0, Text::AlignMode::ToMiddle, { 255, 255, 255 }),
				});
			itemDescription->Initialize(
				Initializer.SCREEN_WIDTH / 2,
				Initializer.SCREEN_HEIGHT / 2 - itemDescription->GetHeight() / 2);
		};
		items.push_back(item);

		item = new Item(35,
			"scorching_sword",
			L"Опаляющий меч",
			Initializer.tilesetFloor,
			{
				4 * Initializer.TEXTURE_RES,
				7 * Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES,
				Initializer.TEXTURE_RES
			});
		item->nameColor = epic;
		item->isConsumable = false;
		item->isEquippable = true;
		item->requiresTile = false;
		item->transferToNextDay = true;
		item->CalculateSpawnRate =
			[&](TileObject* sender)
		{
			return 0;
		};
		item->OnItemUse = [&](TileObject* sender) {};
		item->OnItemReceive = [&](TileObject* sender) {};
		item->OnItemTrash = [&](TileObject* sender) {};
		item->OnItemEquip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage += 25 + Controller.sessionDay * 12;
				Controller.RecalculateOverlay();
			}
		};
		item->OnItemUnequip = [&](TileObject* sender)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				_sender->damage = MoveTowards(_sender->damage, 1, 25 + Controller.sessionDay * 12);
				Controller.RecalculateOverlay();
			}
		};
		item->OnAttackWith = [&](TileObject* sender, TileObject* receiver)
		{
			auto _sender = dynamic_cast<Creature*>(sender);
			if (_sender != nullptr)
			{
				Tile* t;
				Creature* target;
				switch (_sender->facingDirection)
				{
				case Up:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x - 1, _sender->y - 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y - 1);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.075, 7,
							{
								new AnimationFrame(48, 16, 3 * 16, 3 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 3 * 16, 4 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr)
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 16, 3 * 16, 5 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 3 * 16, 6 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 3 * 16, 7 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 3 * 16, 8 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr)
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 16, 3 * 16, 9 * 16, 0.5, 0.5)
							}),
						"amethyst_scythe",
						"textures/amethyst_scythe_visual_effect.png",
						7, _sender->x, _sender->y - 1
						));
					break;
				}
				case Down:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x - 1, _sender->y + 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y + 1);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.075, 7,
							{
								new AnimationFrame(48, 16, 0 * 16, 9 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 0 * 16, 8 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr)
												Creature::DealDamage(25 + Controller.sessionDay * 12, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 16, 0 * 16, 7 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 0 * 16, 6 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 0 * 16, 5 * 16, 0.5, 0.5),
								new AnimationFrame(48, 16, 0 * 16, 4 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr)
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(48, 16, 0 * 16, 3 * 16, 0.5, 0.5)
							}),
						"amethyst_scythe",
						"textures/amethyst_scythe_visual_effect.png",
						7, _sender->x, _sender->y + 1
						));
					break;
				}
				case Right:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x, _sender->y + 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y + 1);
					params.push_back(t);
					
					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x + 1, _sender->y - 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x, _sender->y - 1);
					params.push_back(t);

					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.075, 11,
							{
								new AnimationFrame(16, 48, 0 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 1 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(32, 48, 2 * 16, 0 * 16, 0.25, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[5]))
												Creature::DealDamage(25 + Controller.sessionDay * 12, _target, (Creature*)(_parameters[5]));
										}
									}, params),
								new AnimationFrame(32, 48, 4 * 16, 0 * 16, 0.25, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr)
												Creature::DealDamage(25 + Controller.sessionDay * 12, _target, (Creature*)(_parameters[5]));
										}
									}, params),
								new AnimationFrame(32, 48, 6 * 16, 0 * 16, 0.25, 0.5),
								new AnimationFrame(32, 48, 8 * 16, 0 * 16, 0.25, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[2]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[5]))
												Creature::DealDamage(25 + Controller.sessionDay * 12, _target, (Creature*)(_parameters[5]));
										}
									}, params),
								new AnimationFrame(32, 48, 10 * 16, 0 * 16, 0.25, 0.5),
								new AnimationFrame(32, 48, 12 * 16, 0 * 16, 0.25, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[3]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[5]))
												Creature::DealDamage(25 + Controller.sessionDay * 12, _target, (Creature*)(_parameters[5]));
										}
									}, params),
								new AnimationFrame(32, 48, 14 * 16, 0 * 16, 0.25, 0.5),
								new AnimationFrame(32, 48, 16 * 16, 0 * 16, 0.25, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[4]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr && _target != (Creature*)(_parameters[5]))
												Creature::DealDamage(25 + Controller.sessionDay * 12, _target, (Creature*)(_parameters[5]));
										}
									}, params),
								new AnimationFrame(16, 48, 18 * 16, 0 * 16, 0.5, 0.5)
							}),
						"blazing_swing",
						"textures/blazing_swing_visual_effect.png",
						11, _sender->x, _sender->y
						));
					break;
				}
				case Left:
				{
					vector<void*> params;
					t = Controller.map->tilemap->GetTile(_sender->x - 1, _sender->y - 1);
					params.push_back(t);

					t = Controller.map->tilemap->GetTile(_sender->x - 1, _sender->y + 1);
					params.push_back(t);
					params.push_back(_sender);

					Controller.visualEffects.push_back(new VisualEffect
					(
						new Animation(0.075, 7,
							{
								new AnimationFrame(16, 48, 7 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 8 * 16, 0 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[1]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr)
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(16, 48, 9 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 10 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 11 * 16, 0 * 16, 0.5, 0.5),
								new AnimationFrame(16, 48, 12 * 16, 0 * 16, 0.5, 0.5,
									[=](vector<void*> _parameters)
									{
										Creature* _target;
										Tile* t = (Tile*)(_parameters[0]);
										if (t != nullptr)
										{
											_target = dynamic_cast<Creature*>(t->GetTileObject());
											if (_target != nullptr)
												Creature::DealDamage(30 + Controller.sessionDay * 15, _target, (Creature*)(_parameters[2]));
										}
									}, params),
								new AnimationFrame(16, 48, 13 * 16, 0 * 16, 0.5, 0.5)
							}),
						"amethyst_scythe",
						"textures/amethyst_scythe_visual_effect.png",
						7, _sender->x - 1, _sender->y
						));
					break;
				}
				default: break;
				}
			}
		};
		items.push_back(item);
	}

	Item* GetItemByName(string idName)
	{
		for (auto i = items.begin(); i != items.end(); i++)
			if ((*i)->itemIdName == idName)
				return *i;
		return nullptr;
	}
	Item* GetItemByID(int id)
	{
		for (auto i = items.begin(); i != items.end(); i++)
			if ((*i)->itemID == id)
				return *i;
		return nullptr;
	}
	Item* GetRandomItem(vector<string> idNames)
	{
		Item* result = nullptr;
		vector<string> copy;
		int r;
		do
		{
			copy = vector<string>(idNames);
			while (copy.size() > 1)
				for (int i = 0; i < copy.size() && copy.size() > 0;)
				{
					r = rand() % 1000;
					if (r > GetItemByName(copy[i])->spawnRate)
						copy.erase(copy.begin() + i);
					else i++;
				}
			if (copy.size() > 0)
				result = GetItemByName(copy[0]);
		}
		while (result == nullptr);
		return result;
	}
	Item* GetRandomItemNonPercent(vector<string> idNames)
	{
		return GetItemByName(idNames[rand() % idNames.size()]);
	}
	void RecalculateItems()
	{
		for (auto i = items.begin(); i != items.end(); i++)
		{
			(*i)->spawnRate = (*i)->CalculateSpawnRate(Controller.mainHero);
			(*i)->RebuildDescriptionText(Controller.mainHero, (*i)->itemDescription);
		}
	}
};