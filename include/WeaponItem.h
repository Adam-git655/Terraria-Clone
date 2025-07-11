#pragma once
#include "Item.h"

class WeaponItem : public Item
{
public:
	WeaponItem(const std::string& name, int damage)
		:Item(Item::ItemType::Weapon, name), damage(damage)
	{

	}

private:
	int damage;
};