#pragma once
#include "SFML/Graphics.hpp"
#include "iostream"
#include "ChunksManager.h"

class Weapon
{
public:
	Weapon(float damage, float attackCooldown)
		:damage(damage), attackCooldown(attackCooldown)
	{
	};

	virtual void attack(sf::Vector2f targetPos, ChunksManager& chunksManager) = 0;
	virtual ~Weapon() = default;

protected:
	float damage;
	sf::Clock attackClock;
	float attackCooldown;
};