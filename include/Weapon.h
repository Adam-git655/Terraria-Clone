#pragma once
#include "SFML/Graphics.hpp"
#include "Vec2.h"
#include "iostream"

class Weapon
{
public:
	Weapon(float damage, float attackCooldown)
		:damage(damage), attackCooldown(attackCooldown)
	{
	};

	virtual void attack(Vec2 targetPos) = 0;
	virtual ~Weapon() = default;

protected:
	float damage;
	sf::Clock attackClock;
	float attackCooldown;
};