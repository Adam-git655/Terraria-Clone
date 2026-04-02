#pragma once

struct MovementComponent
{
	float speed = 200.0f;
	float max_speed = 300.0f;
	float jumpStrength = 6.0f;
	bool IsJumping = false;
};