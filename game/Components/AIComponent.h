#pragma once
#include "core/Vec2.h"
#include "SFML/Graphics.hpp"

struct AIComponent
{
	std::vector<IVec2> path;
	int currentPathIndex = 1;
	sf::VertexArray lines;

	IVec2 currentTile;
	IVec2 nextTile;

	int maxJumpHeight = 5;
	float visionRange = 1200.0f;
	float attackRange = 50.0f;
	bool canSeePlayer = false;
	bool canAttackPlayer = false;
};