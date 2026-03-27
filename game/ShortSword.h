#pragma once
#include "Weapon.h"

class ShortSword : public Weapon
{
public:
	ShortSword(float damage, float attackCooldown);

	void attack(sf::Vector2f targetPos, ChunksManager& chunksManager) override;
};