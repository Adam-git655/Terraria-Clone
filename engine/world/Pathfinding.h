#pragma once
#include <vector>
#include "engine/core/Vec2.h"
#include "engine/world/ChunksManager.h"

namespace Pathfinding
{
	struct Node
	{
		float globalGoal;
		float localGoal;
		int x;
		int y;
		IVec2 parent;
	};

	std::vector<IVec2> solveAStar(int startX, int startY, int goalX, int goalY, int maxJumpHeight, ChunksManager& chunksManager);
	
	std::vector<IVec2> getNeighbours(int x, int y, int maxJumpHeight, ChunksManager& chunksManager);

	float distance(int startX, int startY, int endX, int endY)
	{
		return std::abs(startX - endX) + std::abs(startY - endY);
	}

	float heuristic(int startX, int startY, int endX, int endY)
	{
		return std::abs(startX - endX) + std::abs(startY - endY);
	}

	bool isFree(int x, int y, ChunksManager& chunksManager);
	bool isTileBelowGround(int x, int y, ChunksManager& chunksManager);

	struct CompareNode
	{
		bool operator()(Node* a, Node* b)
		{
			return a->globalGoal > b->globalGoal;
		}
	};
}