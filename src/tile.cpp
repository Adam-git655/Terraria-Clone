#include "tile.h"

Tile::Tile(TileType t, bool s)
	:type(t), Solid(s), lightColor(sf::Color::White)
{
}

Tile::TileType Tile::getType() const
{
	return type;
}

void Tile::setType(TileType t)
{
	type = t;
}

bool Tile::isSolid() const
{
	return Solid;
}

void Tile::setSolid(bool s)
{
	Solid = s;
}

void Tile::setTileSprite(sf::Texture tex)
{
	tileSprite.setTexture(tex);
}

sf::Sprite Tile::getSprite() const
{
	return tileSprite;
}

void Tile::setLightColor(sf::Color color)
{
	lightColor = color;
}

sf::Color Tile::getLightColor() const
{
	return lightColor;
}

Vec2 Tile::getPosition() const
{
	return Vec2(tileSprite.getPosition().x, tileSprite.getPosition().y);
}

void Tile::setPosition(Vec2 p)
{
	tileSprite.setPosition(p.x, p.y);
}

sf::FloatRect Tile::getBounds() const
{
	return sf::FloatRect(tileSprite.getPosition().x, tileSprite.getPosition().y, tileSize, tileSize);
}

std::string Tile::tileTypeToString(Tile::TileType type)
{
	switch (type)
	{
	case Tile::TileType::Air:
		return "Air";
	case Tile::TileType::Grass:
		return "Grass";
	case Tile::TileType::Dirt:
		return "Dirt";
	case Tile::TileType::CaveAir:
		return "CaveAir";
	case Tile::TileType::Stone:
		return "Stone";
	case Tile::TileType::Wood:
		return "Wood";
	case Tile::TileType::Leaf:
		return "Leaf";
	default:
		break;
	}
}