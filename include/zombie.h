#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <cassert>
#include "Entity.h"
#include "ChunksManager.h"

struct Node
{
	float globalGoal;
	float localGoal;
	int x;
	int y;
	IVec2 parent;
};

class Zombie : public Entity
{
public:

	Zombie(Vec2 p = Vec2(0.0f, 0.0f));

	void update(float dt, Player& player, ChunksManager& chunksManager, sf::RenderWindow& window);
	void takeDamage(float damage);
	bool isAlive() const;

	std::vector<IVec2> solveAStar(int startX, int startY, int goalX, int goalY, int maxJumpHeight, ChunksManager& chunksManager);

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

	float distance(int startX, int startY, int endX, int endY)
	{
		return std::abs(startX - endX) + std::abs(startY - endY);
	}

	float heuristic(int startX, int startY, int endX, int endY)
	{
		return std::abs(startX - endX) + std::abs(startY - endY);
	}

	std::vector<IVec2> getNeighbours(int x, int y, int maxJumpHeight, ChunksManager& chunksManager);

	struct CompareNode
	{
		bool operator()(Node* a, Node* b)
		{
			return a->globalGoal > b->globalGoal;
		}
	};

};