#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <unordered_set>
#include "Entity.h"
#include "ChunksManager.h"

struct Node
{
	float globalGoal;
	float localGoal;
	int x;
	int y;
	Vec2 parent;
};

class Zombie : public Entity
{
public:

	Zombie(Vec2 p = Vec2(0.0f, 0.0f));

	void update(float dt, Player& player, ChunksManager& chunksManager);
	void takeDamage(float damage);
	bool isAlive() const;

	std::vector<Vec2> solveAStar(int startX, int startY, int goalX, int goalY, int maxJumpHeight, ChunksManager& chunksManager);

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

	float distance(Vec2 start, Vec2 end)
	{
		return std::abs(start.x - end.x) + std::abs(start.y - end.y);
	}

	float heuristic(Vec2 start, Vec2 end)
	{
		return std::abs(start.x - end.x) * 2 + std::abs(start.y - end.y);
	}

	std::vector<Vec2> getNeighbours(int x, int y, int maxJumpHeight, ChunksManager& chunksManager);

	struct CompareNode
	{
		bool operator()(Node* a, Node* b)
		{
			return a->globalGoal > b->globalGoal;
		}
	};

};