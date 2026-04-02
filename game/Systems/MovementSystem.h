#pragma once

#include "engine/ecs/EntityManager.h"
#include "game/Components/TransformComponent.h"
#include "game/Components/MovementComponent.h"
#include "game/Components/PhysicsComponent.h"
#include "game/Components/InputComponent.h"
#include "game/Components/RenderComponent.h"
#include "game/Components/AIComponent.h"

class MovementSystem
{
public:
	void update(EntityManager& mgr, float dt);

private:
	void zombieFollowPath(PhysicsComponent& physics, MovementComponent& movement, RenderComponent& render, const IVec2& currentTile, const IVec2& nextTile);
};
