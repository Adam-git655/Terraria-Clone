#pragma once

struct AirAIComponent
{
	float visionRange = 1200.0f;
	float attackRange = 100.0f;
	bool canSeePlayer = false;
	bool canAttackPlayer = false;
	Vec2 dir{ 0, 0 };
};