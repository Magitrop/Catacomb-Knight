#pragma once

#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()
#define Overlay UIOverlayHandler::Instance()

#include <SDL.h>
#include <iostream>
#include <functional>
#include <vector>
#include <windows.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include "MapController.h"
#include "Creature.h"
#include "UIOverlayHandler.h"
#include "Text.h"

using namespace std;

class Item final
{
public:
	string itemIdName;
	wstring itemName;
	TextsContainer* itemDescription;
	SDL_Texture* itemTexture;
	SDL_Rect textureSrc;
	SDL_Color nameColor;
	bool isConsumable;
	bool isEquippable;
	bool transferToNextDay;
	bool requiresTile;
	bool worksOnlyIfEquipped;
	bool canUseInFogOfWar;
	int itemID;
	int spawnRate;
	int usingRange;

	function<int(TileObject* owner)> CalculateSpawnRate;
	function<void(TileObject* owner, TextsContainer* &itemDescription)> RebuildDescriptionText;
	function<void(TileObject* owner)> OnItemReceive;
	function<void(TileObject* owner)> OnItemTrash;
	function<void(TileObject* owner)> OnItemUse;
	function<void(TileObject* owner, Tile* at)> OnItemUseAt;
	function<void(TileObject* owner)> OnItemEquip;
	function<void(TileObject* owner)> OnItemUnequip;
	function<void(TileObject* owner, TileObject* receiver)> OnAttackWith;
	function<void(TileObject* owner, int amount, TileObject* from, int* totalAmountChange)> OnGettingDamageWith;

	Item(int _itemID, string idName, wstring _name, SDL_Texture* _texture, SDL_Rect _src)
	{
		nameColor = { 255, 255, 255 };
		itemID = _itemID;
		itemIdName = idName;
		itemName = _name;
		itemTexture = _texture;
		textureSrc = _src;
		OnItemUseAt = [&](TileObject* sender, Tile* at) {};
		RebuildDescriptionText = [&](TileObject* sender, TextsContainer* &itemDescription) {};
		OnGettingDamageWith = [&](TileObject* sender, int amount, TileObject* from, int* totalAmountChange) {};
		worksOnlyIfEquipped = false;
	}
	Item(const Item& i)
	{
		itemID = i.itemID;
		itemIdName = i.itemIdName;
		itemName = i.itemName;
		itemTexture = i.itemTexture;
		textureSrc = i.textureSrc;
		isConsumable = i.isConsumable;
		isEquippable = i.isEquippable;
		canUseInFogOfWar = i.canUseInFogOfWar;
		transferToNextDay = i.transferToNextDay;
		requiresTile = i.requiresTile;
		spawnRate = i.spawnRate;
		usingRange = i.usingRange;
		nameColor = i.nameColor;
		worksOnlyIfEquipped = i.worksOnlyIfEquipped;
		itemDescription = i.itemDescription;

		OnAttackWith = i.OnAttackWith;
		OnGettingDamageWith = i.OnGettingDamageWith;
		CalculateSpawnRate = i.CalculateSpawnRate;
		OnItemReceive = i.OnItemReceive;
		OnItemTrash = i.OnItemTrash;
		OnItemUse = i.OnItemUse;
		OnItemUseAt = i.OnItemUseAt;
		OnItemEquip = i.OnItemEquip;
		OnItemUnequip = i.OnItemUnequip;
	}
};

class InventoryHandler final
{
private:
	InventoryHandler() {}
	~InventoryHandler()
	{
		for (auto i = slots.begin(); i != slots.end(); i++)
			delete *i;
	}
	InventoryHandler(InventoryHandler& mm) {}
	InventoryHandler& operator = (InventoryHandler& mm) {}

	int _mouseX, _mouseY;
public:
	static InventoryHandler& Instance()
	{
		static InventoryHandler instance;
		return instance;
	}

	class InventorySlot final
	{
	private:
		Item* item;
	public:
		SDL_Rect slotRenderRect;
		SDL_Rect itemRenderRect;
		bool mouseIn;
		Text* slotTipText;

		~InventorySlot()
		{
			RemoveSlot();
		}
		InventorySlot(SDL_Rect _slotRect, SDL_Rect _itemRect, Text* text)
		{
			item = nullptr;
			slotTipText = text;
			slotTipText->SetRenderCoordinates(1, 1);
			slotRenderRect = _slotRect;
			itemRenderRect = _itemRect;
		}
		void PlaceItem(Item* _itemToPlace)
		{
			if (item != nullptr)
				return;
			item = _itemToPlace;
			if (item != nullptr)
			{
				slotTipText->color = item->nameColor;
				slotTipText->RebuildText();
			}
		}
		Item* PullItem()
		{
			auto _item = item;
			item = nullptr;
			return _item;
		}
		Item* GetItem()
		{
			return item;
		}
		void UseItem()
		{
			if (item == nullptr)
				return;
			GetItem()->OnItemUse(Controller.mainHero);
			if (item->isConsumable)
			{
				delete item;
				item = nullptr;
			}
		}
		void UseItem(Tile* at)
		{
			if (item == nullptr)
				return;
			GetItem()->OnItemUseAt(Controller.mainHero, at);
			if (item->isConsumable)
			{
				delete item;
				item = nullptr;
			}
		}
		void RemoveSlot()
		{
			if (item != nullptr)
				delete item;
			item = nullptr;
			delete slotTipText;
		}
		void OnMouseEnter()
		{
			mouseIn = true;
		}
		void OnMouseOut()
		{
			mouseIn = false;
		}
	};

	int rowsCount = 3;
	int columnsCount = 5;
	int inventorySizeX;
	int inventorySizeY;

	int slotVisualSize = 75;
	int itemVisualSize = 60;

	vector<InventorySlot*> slots;
	InventorySlot* equipmentSlot;

	SDL_Texture* inventoryTexture;
	SDL_Rect slotTextureSrc;
	SDL_Rect selectedSlotTextureSrc;
	SDL_Rect equipmentSlotTextureSrc;
	SDL_Rect selectedEquipmentSlotTextureSrc;
	SDL_Rect itemTrashTipSrc;
	SDL_Rect buttonShowInventorySrc;
	SDL_Rect descriptionBackgroundSrc;

	bool isDraggingItem;
	InventorySlot* dragFrom;
	// for dragging
	InventorySlot* selectedSlot;
	// for item use
	InventorySlot* currentSlot;

	Text* hintText;
	bool inventoryIsShown;

	void Initialize()
	{
		for (auto i = slots.begin(); i != slots.end(); i++)
			delete *i;
		slots.clear();
		for (int r = rowsCount; r > 0; r--)
			for (int c = 0; c < columnsCount; c++)
				slots.push_back(new InventorySlot(
					{
						c * slotVisualSize + (c + 2) * slotVisualSize / 8,
						Initializer.SCREEN_HEIGHT - r * slotVisualSize - (r + 1) * slotVisualSize / 8,
						slotVisualSize,
						slotVisualSize
					},
					{
						c * slotVisualSize + (slotVisualSize - itemVisualSize) / 2 + (c + 2) * slotVisualSize / 8,
						Initializer.SCREEN_HEIGHT - r * slotVisualSize - (r + 1) * slotVisualSize / 8 + (slotVisualSize - itemVisualSize) / 2,
						itemVisualSize,
						itemVisualSize
					},
					new Text("fonts/pixel_font1.ttf", L"", 60, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 })));
		slots.push_back(equipmentSlot =
			new InventorySlot(
				{
					columnsCount * slotVisualSize + (columnsCount + 2) * slotVisualSize / 8,
					(int)(Initializer.SCREEN_HEIGHT - (rowsCount - 1) * slotVisualSize - rowsCount * slotVisualSize / 8),
					slotVisualSize,
					slotVisualSize
				},
				{
					columnsCount * slotVisualSize + (slotVisualSize - itemVisualSize) / 2 + (columnsCount + 2) * slotVisualSize / 8,
					(int)(Initializer.SCREEN_HEIGHT - (rowsCount - 1) * slotVisualSize - rowsCount * slotVisualSize / 8 + (slotVisualSize - itemVisualSize) / 2),
					itemVisualSize,
					itemVisualSize
				},
				new Text("fonts/pixel_font1.ttf", L"", 60, 20, Text::AlignMode::ToMiddle, { 255, 255, 255 })));
		inventorySizeX = (columnsCount + 2) * (slotVisualSize + slotVisualSize / 8);
		inventorySizeY = (rowsCount + 1) * (slotVisualSize + slotVisualSize / 8);
		inventoryTexture = Overlay.textureUI;
		slotTextureSrc = { 300, 161, 32, 32 };
		selectedSlotTextureSrc = { 332, 161, 32, 32 };
		equipmentSlotTextureSrc = { 300, 193, 32, 32 };
		selectedEquipmentSlotTextureSrc = { 332, 193, 32, 32 };
		itemTrashTipSrc = { 300, 145, 16, 16 };
		buttonShowInventorySrc = { 316, 4 * 16, 16, 16 };
		descriptionBackgroundSrc = { 332, 129, 32, 32 };

		hintText = new Text("fonts/pixel_font1.ttf", L"ЛКМ - перетащить, ПКМ - использовать", 35, 20, Text::AlignMode::ToMiddle, { 255, 255, 0 });
		hintText->SetRenderCoordinates(
			(inventorySizeX - slotVisualSize) / 2,
			Initializer.SCREEN_HEIGHT - (inventorySizeY - slotVisualSize) - 3 * hintText->GetHeight());
	}
	void RenderHint()
	{
		SDL_RenderSetScale(Initializer.renderer, 1, 1);
		hintText->Render(255);
		SDL_RenderSetScale(Initializer.renderer, (float)Controller.map->zoom, (float)Controller.map->zoom);
	}
	bool HasEmptySlot()
	{
		for (auto i = slots.begin(); i != slots.end(); i++)
			if ((*i) != equipmentSlot && (*i)->GetItem() == nullptr)
				return true;
		return false;
	}
	Item* FindItem(string itemName)
	{
		for (auto i = slots.begin(); i != slots.end(); i++)
			if ((*i)->GetItem() != nullptr && (*i)->GetItem()->itemIdName == itemName)
				return (*i)->GetItem();
		return nullptr;
	}
	InventorySlot* FindSlotWithItem(string itemName)
	{
		for (auto i = slots.begin(); i != slots.end(); i++)
			if ((*i)->GetItem() != nullptr && (*i)->GetItem()->itemIdName == itemName)
				return *i;
		return nullptr;
	}
	void SwapItems(InventorySlot* from, InventorySlot* to)
	{
		Item* itemFrom = from->PullItem();

		from->PlaceItem(to->PullItem());
		if (from->GetItem() != nullptr)
		{
			from->slotTipText->text = from->GetItem()->itemName;
			from->slotTipText->RebuildText();
		}

		to->PlaceItem(itemFrom);
		if (to->GetItem() != nullptr)
		{
			to->slotTipText->text = to->GetItem()->itemName;
			to->slotTipText->RebuildText();
		}
	}
	void AddItemToInventory(Item* itemToAdd)
	{
		if (itemToAdd == nullptr)
			return;
		for (auto i = slots.begin(); i != slots.end(); i++)
		{
			if ((*i)->GetItem() == nullptr)
			{
				(*i)->PlaceItem(itemToAdd);
				(*i)->GetItem()->OnItemReceive(Controller.mainHero);
				if ((*i) == equipmentSlot)
					(*i)->GetItem()->OnItemEquip(Controller.mainHero);
				(*i)->slotTipText->text = (*i)->GetItem()->itemName;
				(*i)->slotTipText->RebuildText();
				break;
			}
		}
	}
	void AddItemToSlot(Item* itemToAdd, InventorySlot* slot)
	{
		if (itemToAdd == nullptr)
			return;
		if (slot->GetItem() == nullptr)
		{
			slot->PlaceItem(itemToAdd);
			slot->GetItem()->OnItemReceive(Controller.mainHero);
			if (slot == equipmentSlot)
				slot->GetItem()->OnItemEquip(Controller.mainHero);
			slot->slotTipText->text = slot->GetItem()->itemName;
			slot->slotTipText->RebuildText();
		}
	}
	void Render()
	{
		if (inventoryIsShown)
		{
			SDL_Rect rect;
			SDL_RenderSetScale(Initializer.renderer, 1, 1);
			for (auto i = slots.begin(); i != slots.end(); i++)
				if (currentSlot != nullptr && currentSlot == (*i))
				{
					if ((*i) == equipmentSlot && (*i)->GetItem() == nullptr)
						SDL_RenderCopy(Initializer.renderer, inventoryTexture, &selectedEquipmentSlotTextureSrc, &(*i)->slotRenderRect);
					else 
						SDL_RenderCopy(Initializer.renderer, inventoryTexture, &selectedSlotTextureSrc, &(*i)->slotRenderRect);
				}
				else
				{
					if ((*i) == equipmentSlot && (*i)->GetItem() == nullptr)
						SDL_RenderCopy(Initializer.renderer, inventoryTexture, &equipmentSlotTextureSrc, &(*i)->slotRenderRect);
					else
						SDL_RenderCopy(Initializer.renderer, inventoryTexture, &slotTextureSrc, &(*i)->slotRenderRect);
				}
			for (auto i = slots.begin(); i != slots.end(); i++)
			{
				if ((*i)->GetItem() != nullptr)
				{
					if ((*i) == dragFrom)
					{
						rect =
						{
							_mouseX,
							_mouseY,
							itemVisualSize,
							itemVisualSize
						};
						SDL_RenderCopy(Initializer.renderer, (*i)->GetItem()->itemTexture, &(*i)->GetItem()->textureSrc, &rect);
						if (!(_mouseX < inventorySizeX &&
							_mouseY > Initializer.SCREEN_HEIGHT - inventorySizeY))
							SDL_RenderCopy(Initializer.renderer, inventoryTexture, &itemTrashTipSrc, &rect);
					}
					else SDL_RenderCopy(Initializer.renderer, (*i)->GetItem()->itemTexture, &(*i)->GetItem()->textureSrc, &(*i)->itemRenderRect);
				}
			}

			rect =
			{
				columnsCount * slotVisualSize + (columnsCount + 2) * slotVisualSize / 8,
				(int)(Initializer.SCREEN_HEIGHT - rowsCount * slotVisualSize - (rowsCount + 1) * slotVisualSize / 8),
				35,
				35
			};
			SDL_RenderCopy(Initializer.renderer, Menu.textureUI, &buttonShowInventorySrc, &rect);

			for (auto i = slots.begin(); i != slots.end(); i++)
				if ((*i)->GetItem() != nullptr && (*i) != dragFrom)
					if ((*i)->mouseIn && (*i)->GetItem()->itemDescription != nullptr)
					{
						int width = (*i)->GetItem()->itemDescription->GetWidth() + 30;
						int height = (*i)->GetItem()->itemDescription->GetHeight() + 30;
						rect =
						{
							(Initializer.SCREEN_WIDTH - width) / 2,
							(Initializer.SCREEN_HEIGHT - height + 50) / 2,
							width,
							height
						};
						SDL_RenderCopy(Initializer.renderer, Menu.textureUI, &descriptionBackgroundSrc, &rect);
						(*i)->GetItem()->itemDescription->Render(255);
						//(*i)->slotTipText->Render(255);
					}
			SDL_RenderSetScale(Initializer.renderer, (float)Controller.map->zoom, (float)Controller.map->zoom);
		}
		/*else
		{
			SDL_RenderSetScale(Initializer.renderer, 1, 1);
			SDL_Rect rect =
			{
				10,
				Initializer.SCREEN_HEIGHT - 45,
				35,
				35
			};
			SDL_RenderCopy(Initializer.renderer, Menu.textureUI, &buttonShowInventorySrc, &rect);
			SDL_RenderSetScale(Initializer.renderer, (float)Controller.map->zoom, (float)Controller.map->zoom);
		}*/
	}
	void CheckMousePosition(int mouseX, int mouseY)
	{
		_mouseX = mouseX;
		_mouseY = mouseY;
		int render;
		SDL_RenderSetScale(Initializer.renderer, 1, 1);
		for (auto i = slots.begin(); i != slots.end(); i++)
		{
			if (mouseX >= (*i)->slotRenderRect.x &&
				mouseX <= (*i)->slotRenderRect.x + (*i)->slotRenderRect.w &&
				mouseY >= (*i)->slotRenderRect.y &&
				mouseY <= (*i)->slotRenderRect.y + (*i)->slotRenderRect.h)
			{
				(*i)->OnMouseEnter();
				(*i)->slotTipText->SetRenderCoordinatesWithoutRerender(
					clamp(render = mouseX,
						(*i)->slotTipText->GetWidth() / 2 + 10,
						Initializer.SCREEN_WIDTH - (*i)->slotTipText->GetWidth() / 2 - 10
					),
					clamp(render = mouseY,
						0,
						Initializer.SCREEN_HEIGHT - 2 * (*i)->slotTipText->GetHeight()
					));
			}
			else if ((*i)->mouseIn)
				(*i)->OnMouseOut();
		}
		SDL_RenderSetScale(Initializer.renderer, (float)Controller.map->zoom, (float)Controller.map->zoom);
	}
	void CheckMouseButtonDown(char mouseButton)
	{
		for (auto i = slots.begin(); i != slots.end(); i++)
			if ((*i)->mouseIn)
			{
				if (mouseButton == SDL_BUTTON_LEFT && (*i)->GetItem() != nullptr)
				{
					dragFrom = (*i);
					isDraggingItem = true;
					selectedSlot = nullptr;
				}
				else if (mouseButton == SDL_BUTTON_RIGHT && !isDraggingItem)
					selectedSlot = (*i);
				return;
			}
		selectedSlot = nullptr;
	}
	void CheckMouseButtonUp(char mouseButton)
	{
		for (auto i = slots.begin(); i != slots.end(); i++)
			if ((*i)->mouseIn)
			{
				if (mouseButton == SDL_BUTTON_LEFT)
				{
					if (dragFrom != nullptr && (*i) != dragFrom)
					{
						if ((*i) != equipmentSlot && dragFrom != equipmentSlot)
							SwapItems(dragFrom, (*i));
						else if ((*i) == equipmentSlot && dragFrom->GetItem()->isEquippable)
						{
							dragFrom->GetItem()->OnItemEquip(Controller.mainHero);
							if ((*i)->GetItem() != nullptr)
								(*i)->GetItem()->OnItemUnequip(Controller.mainHero);
							SwapItems(dragFrom, (*i));
						}
						else if (dragFrom == equipmentSlot)
						{
							if ((*i)->GetItem() != nullptr)
							{
								if ((*i)->GetItem()->isEquippable)
								{
									dragFrom->GetItem()->OnItemUnequip(Controller.mainHero);
									(*i)->GetItem()->OnItemEquip(Controller.mainHero);
									SwapItems(dragFrom, (*i));
								}
							}
							else
							{
								dragFrom->GetItem()->OnItemUnequip(Controller.mainHero);
								SwapItems(dragFrom, (*i));
							}
						}
					}
					isDraggingItem = false;
					dragFrom = nullptr;
				}
				else if (mouseButton == SDL_BUTTON_RIGHT && !isDraggingItem)
				{
					if (Controller.currentCreature == Controller.mainHero &&
						(*i) == selectedSlot &&
						(*i)->GetItem() != nullptr)
					{
						if ((*i)->GetItem()->requiresTile)
						{
							if (!(*i)->GetItem()->worksOnlyIfEquipped ||
								((*i)->GetItem()->worksOnlyIfEquipped && (*i) == equipmentSlot))
							{
								currentSlot = selectedSlot;
								Controller.BeginTileSelection();
							}
						}
						else
						{
							UseItem(*i);
							/*if (selectedSlot == equipmentSlot && (*i)->GetItem()->isConsumable)
								(*i)->GetItem()->OnItemUnequip(Controller.mainHero);
							selectedSlot->UseItem();*/
						}
					}
				}
			}
		if (mouseButton == SDL_BUTTON_LEFT)
		{
			if (!(_mouseX < inventorySizeX &&
				_mouseY > Initializer.SCREEN_HEIGHT - inventorySizeY) &&
				isDraggingItem &&
				dragFrom != nullptr &&
				dragFrom->GetItem() != nullptr)
			{
				if (dragFrom == equipmentSlot)
					dragFrom->GetItem()->OnItemUnequip(Controller.mainHero);
				dragFrom->GetItem()->OnItemTrash(Controller.mainHero);
				delete dragFrom->PullItem();
			}
			isDraggingItem = false;
			dragFrom = nullptr;
		}
		Tile* tile = Controller.map->tilemap->GetTile(Controller.mainHero->x, Controller.mainHero->y);
		if (tile->tileItem)
			if (HasEmptySlot())
				tile->PickUpTileItem(Controller.mainHero, tile->tileItem->connectedItemIdName);
		Controller.RecalculateOverlay();
		selectedSlot = nullptr;
	}
	void UseItem(InventorySlot* slot)
	{
		if (slot == nullptr)
			return;
		if (selectedSlot == equipmentSlot && slot->GetItem()->isConsumable)
			slot->GetItem()->OnItemUnequip(Controller.mainHero);
		selectedSlot->UseItem();
	}
	void UseItem(InventorySlot* slot, Tile* targetTile)
	{
		if (slot == nullptr || targetTile == nullptr)
			return;
		if (slot == equipmentSlot && slot->GetItem()->isConsumable)
			slot->GetItem()->OnItemUnequip(Controller.mainHero);
		slot->UseItem(targetTile);
	}
};