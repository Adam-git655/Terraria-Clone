#pragma once
#include "engine/core/Vec2.h"

struct PhysicsComponent
{
	Vec2 velocity = { 0.0f, 0.0f };
	float gravity = 1400.0f;
	float terminalVelocity = 2000.0f;
	bool IsOnGround = false;
	bool IsFalling = true;
};