#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Entity.h"
#include "ChunksManager.h"

class Zombie : public Entity
{
public:

	Zombie(Vec2 p = Vec2(0.0f, 0.0f));

	void update(float dt, Player& player, ChunksManager& chunksManager);
	void takeDamage(float damage);
	bool isAlive() const;

private:

	sf::Texture tex;

	float visionRange;
	bool canSeePlayer(const Vec2& playerPos) const;

	bool alive = true;

	sf::Clock attackClock;
	float attackRange;
	float attackCooldown;
	float damageAmount;
	bool canAttackPlayer(const Vec2& playerPos) const;
	void attackPlayer(Player& player) const;
};