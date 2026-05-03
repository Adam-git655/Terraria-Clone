#pragma once
#include "core/Vec2.h"
#include "engine/ecs/EntityManager.h"
#include "game/Components/HealthComponent.h"
#include "game/Components/FactionComponent.h"
#include "game/Components/TransformComponent.h"
#include "game/Components/AnimationComponent.h"

class HealthSystem
{
public:
	void update(EntityManager& mgr, Vec2& playerSpawnPos);
};