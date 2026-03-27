#pragma once
#include "Item.h"

class WeaponItem : public Item
{
public:
	WeaponItem(const std::string& name)
		:Item(Item::ItemType::Weapon, name)
	{

	}
};