#pragma once
#include "iostream"
#include "engine/ecs/EntityManager.h"
#include "game/Components/TransformComponent.h"
#include "game/Components/MovementComponent.h"
#include "game/Components/PhysicsComponent.h"
#include "game/Components/InputComponent.h"
#include "game/Components/RenderComponent.h"
#include "game/Components/AIComponent.h"
#include "game/Components/AirAIComponent.h"

#include "SoundManager.h"

class MovementSystem
{
public:
	void update(EntityManager& mgr, SoundManager& soundMgr, float dt);

private:
	void zombieFollowPathAI(PhysicsComponent& physics, MovementComponent& movement, RenderComponent& render, const IVec2& currentTile, const IVec2& nextTile);
	void zombieFollowPathDumb(Vec2 playerPos, TransformComponent& transform, PhysicsComponent& physics, MovementComponent& movement, RenderComponent& render, AIComponent& ai, float dt);
};
