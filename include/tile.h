#pragma once
#include "Vec2.h"
#include "SFML/Graphics.hpp"

class Tile
{

public:
	enum class TileType
	{
		Air,
		Grass,
		Dirt,
		Stone,
		Wood,
		Leaf
	};


	Tile(TileType t = TileType::Air, bool s = false);

	TileType getType() const;
	void setType(TileType t);

	bool isSolid() const;
	void setSolid(bool s);

	void setTileSprite(sf::Texture tex);
	sf::Sprite getSprite() const;

	Vec2 getPosition() const;
	void setPosition(Vec2 p);

	sf::FloatRect getBounds() const;
	static std::string tileTypeToString(Tile::TileType type);

private:
	sf::Sprite tileSprite;
	int tileSize = 32;

	TileType type;
	bool Solid;

};