#pragma once
#include <string>
#include <array>

struct ItemStack
{
	std::string itemId = "";
	int count = 0;
};

struct InventoryComponent
{
	static const int HOTBAR_SIZE = 9;
	static const int INVENTORY_SIZE = 40;
	int activeHotbarSlot = 0;
	int selectedInventorySlot = -1;
	std::array<ItemStack, HOTBAR_SIZE + INVENTORY_SIZE> slots; //First 9 slots for hotbar
};
