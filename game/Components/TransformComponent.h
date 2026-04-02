#pragma once
#include "engine/core/Vec2.h"

struct TransformComponent
{
	Vec2 position = { 0.0f, 0.0f };
	Vec2 prevPos = { 0.0f, 0.0f };
};