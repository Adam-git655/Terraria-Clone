#include "InventorySystem.h"

void InventorySystem::addItem(EntityManager& mgr, Entt e, const std::string& itemId, int amount, std::unordered_map<std::string, ItemDef>& itemRegistry)
{
	auto& invStorage = mgr.getComponentStorage<InventoryComponent>();
	auto& inv = invStorage.get(e);

	//Add to existing item on hotbar
	for (auto& slot : inv.hotbar)
	{
		if (slot.itemId == itemId && slot.count > 0)
		{
			slot.count = addAmount(slot.count, amount, itemRegistry[itemId].maxStackSize);
			return;
		}
	}

	//If doesnt exist, add to first empty slot
	for (auto& slot : inv.hotbar)
	{
		if (slot.count == 0)
		{
			slot.itemId = itemId;
			slot.count = addAmount(slot.count, amount, itemRegistry[itemId].maxStackSize);
			return;
		}
	}
}

void InventorySystem::removeItem(EntityManager& mgr, Entt e, const std::string& itemId, int amount)
{
	auto& invStorage = mgr.getComponentStorage<InventoryComponent>();
	auto& inv = invStorage.get(e);

	//Remove from item if exists in hotbar
	for (auto& slot : inv.hotbar)
	{
		if (slot.itemId == itemId)
		{
			slot.count = removeAmount(slot.count, amount);
			if (slot.count == 0)
				slot.itemId = "";
			return;
		}
	}
}

int InventorySystem::addAmount(int existingCount, int amount, int maxStackSize)
{
	return std::min(existingCount + amount, maxStackSize);
}

int InventorySystem::removeAmount(int existingCount, int amount)
{
	return std::max(existingCount - amount, 0);
}