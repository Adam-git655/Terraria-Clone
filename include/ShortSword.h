#pragma once
#include "Weapon.h"

class ShortSword : public Weapon
{
public:
	ShortSword(float damage, float attackCooldown);

	void attack(Vec2 targetPos) override;
};