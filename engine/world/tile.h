#pragma once
#include "engine/core/Vec2.h"
#include "iostream"
#include "SFML/Graphics.hpp"

class Tile
{
public:
	enum class TileType
	{
		Air,
		Grass,
		Dirt,
		CaveAir,
		Stone,
		Wood,
		Leaf,
		Torch,
		Sand,
		SandStone,
		Snow,
		Ice,
		SnowLeaf
	};


	Tile(TileType t = TileType::Air, bool s = false);

	TileType getType() const;
	void setType(TileType t);

	bool isSolid() const;
	void setSolid(bool s);

	void setTileSprite(sf::Texture tex);
	sf::Sprite getSprite() const;

	void setLightColor(sf::Color color);
	sf::Color getLightColor() const;

	Vec2 getPosition() const;
	void setPosition(Vec2 p);

	sf::FloatRect getBounds() const;
	static std::string tileTypeToItemId(Tile::TileType type);

private:
	sf::Sprite tileSprite;
	int tileSize = 32;

	TileType type;
	bool Solid;

	sf::Color lightColor;
};

struct TileDef
{
	Tile::TileType type = Tile::TileType::Air;
	bool isSolid = true;
};