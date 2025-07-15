#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "tile.h"
#include "Weapon.h"
#include "Entity.h"

class ChunksManager;

class Player : public Entity
{
private:
	sf::Texture tex;

	sf::Texture shortSwordTex;

	Tile::TileType blockTypeInHand = Tile::TileType::Stone;
	
	bool movement_keys[sf::Keyboard::KeyCount] = { false };

	float health = 100.0f;

	std::unique_ptr<Weapon> weapon;

public:
	Player(Vec2 p = Vec2(400.0f, 400.0f));

	Tile::TileType getBlockTypeInHand() const;

	const sf::Texture& getWeaponTexture(std::string& weaponName) const;

	void setBlockTypeInHand(Tile::TileType type);

	void set_movement_key(int code, bool b);

	void takeDamage(float damage);

	void update(float dt, ChunksManager& chunksManager);

	void equipWeapon(std::unique_ptr<Weapon> weaponToEquip);
	void unequipWeapon();
	bool hasWeaponEquipped() const;
	void handleWeaponAttack(Vec2 targetPos);
};