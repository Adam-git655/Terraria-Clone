#pragma once
#include "SFML/Graphics.hpp"

struct WeaponComponent
{
	std::string weaponName = "ShortSword";
	float damage = 10.0f;
	sf::Clock attackClock;
	float attackCooldown = 2.0f;
	float attackRange= 50.0f;
	bool attackRequested = false;
};