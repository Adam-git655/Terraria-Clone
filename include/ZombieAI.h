#pragma once
#include "Vec2.h"
#include <queue>
#include <set>
#include <unordered_set>
#include <cassert>
#include "SFML/Graphics.hpp"

struct Node
{
	float globalGoal;
	float localGoal;
	int x;
	int y;
	IVec2 parent;
};

class Zombie;
class ChunksManager;

class ZombieAI
{
public:
	ZombieAI(Zombie& z);

	void update(Vec2& playerPos, ChunksManager& chunksManager, sf::RenderWindow& window, float dt);

private:

	Zombie& controlledZombie;

	std::vector<IVec2> path;
	int currentPathIndex = 1;

	float distance(int startX, int startY, int endX, int endY)
	{
		return std::abs(startX - endX) + std::abs(startY - endY);
	}

	float heuristic(int startX, int startY, int endX, int endY)
	{
		return std::abs(startX - endX) + std::abs(startY - endY);
	}

	bool isAtTileCenter(const Vec2& position, const IVec2& tilePos, const float tolerance = 1.0f);

	bool isFree(int x, int y, ChunksManager& chunksManager);
	bool isTileBelowGround(int x, int y, ChunksManager& chunksManager);

	std::vector<IVec2> getNeighbours(int x, int y, int maxJumpHeight, ChunksManager& chunksManager);

	std::vector<IVec2> solveAStar(int startX, int startY, int goalX, int goalY, int maxJumpHeight, ChunksManager& chunksManager);

	struct CompareNode
	{
		bool operator()(Node* a, Node* b)
		{
			return a->globalGoal > b->globalGoal;
		}
	};

};