#pragma once
#include "SFML/Graphics.hpp"

struct WeaponComponent
{
	std::string weaponName = "ShortSword";
	float damage;
	sf::Clock attackClock;
	float attackCooldown;
	float attackRange;
	bool attackRequested = false;
};