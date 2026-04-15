#pragma once
#include "Item.h"
#include "game/Components/InventoryComponent.h"
#include "engine/ecs/EntityManager.h"

class InventorySystem
{
public:
	void addItem(EntityManager& mgr, Entt e, const std::string& itemId, int amount, std::unordered_map<std::string, ItemDef>& itemRegistry);

	void removeItem(EntityManager& mgr, Entt e, const std::string& itemId, int amount);

private:
	int addAmount(int existingCount, int amount, int maxStackSize);
	int removeAmount(int existingCount, int amount);
};
