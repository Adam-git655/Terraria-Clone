#pragma once
#include "core/Vec2.h"
#include "SFML/Graphics.hpp"

struct AIComponent
{
	std::vector<IVec2> path;
	int currentPathIndex = 1;
	sf::VertexArray lines;
	bool hasActivePath = false;

	IVec2 currentTile;
	IVec2 nextTile;

	int maxJumpHeight = 5;
	float visionRange = 1200.0f;
	float attackRange = 50.0f;
	float pathRefreshTimer = 0.0f;
	float pathRefreshInterval = 0.05f;
	bool canSeePlayer = false;
	bool canAttackPlayer = false;

	//for dumb position follow
	float stuckTimer = 0.0f;
	float stuckThreshold = 0.35f;
};