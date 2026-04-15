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
	int activeHotbarSlot = 0;
	std::array<ItemStack, HOTBAR_SIZE> hotbar;
};
