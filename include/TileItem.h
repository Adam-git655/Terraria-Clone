#pragma once
#include "Item.h"
#include "tile.h"

class TileItem : public Item
{
public:
	TileItem(Tile::TileType tileType)
		:Item(Item::ItemType::Tile, Tile::tileTypeToString(tileType)), tileType(tileType)
	{

	}

	Tile::TileType getTileType() const
	{
		return tileType;
	}

private:
	Tile::TileType tileType;
};