#pragma once
#include <string>

class Item
{
public:
	enum class ItemType {
		Tile,
		Weapon
	};

	Item(ItemType type, const std::string& name)
		:type(type), name(name)
	{

	}

	virtual ~Item() = default;

	ItemType getItemType() const { return type; }
	std::string getItemName() const { return name; }

private:
	ItemType type;
	std::string name;
};
