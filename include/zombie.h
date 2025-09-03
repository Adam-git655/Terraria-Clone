#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Entity.h"
#include "ChunksManager.h"
#include "ZombieAI.h"

class Zombie : public Entity
{
public:

	Zombie(Vec2 p = Vec2(0.0f, 0.0f));

	void update(float dt, Player& player, ChunksManager& chunksManager, sf::RenderWindow& window);
	void takeDamage(float damage);
	bool isAlive() const;

	void followPath(const IVec2& currentTile, const IVec2& nextTile, float dt);

private:
	ZombieAI ai;

	sf::Texture tex;
	sf::IntRect rectSourceSprite = { 0, 0, 157, 213 };
	bool walkSpriteSheetGoRight = true;
	sf::Clock zombieWalkClock;

	bool isJumping = false;

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