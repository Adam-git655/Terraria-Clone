#pragma once
#include "Item.h"
#include "tile.h"

class TileItem : public Item
{
public:
	TileItem(Tile::TileType tileType, bool solid)
		:Item(Item::ItemType::Tile, Tile::tileTypeToString(tileType)), tileType(tileType), solid(solid)
	{

	}

	Tile::TileType getTileType() const
	{
		return tileType;
	}

	bool getSolid() const
	{
		return solid;
	}

private:
	Tile::TileType tileType;
	bool solid;
};