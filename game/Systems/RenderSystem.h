#pragma once
#include "engine/ecs/EntityManager.h"
#include "game/Components/RenderComponent.h"
#include "game/Components/TransformComponent.h"
#include "game/Components/AnimationComponent.h"
#include "game/Components/PhysicsComponent.h"
#include "game/Components/MovementComponent.h"
#include "game/Components/AIComponent.h"

class RenderSystem
{
public:
	void update(EntityManager& mgr, float dt);
	void draw(EntityManager& mgr, sf::RenderWindow& window);
	bool showAIDebugLines = false;

private:
	void init(EntityManager& mgr, Entt e, RenderComponent& render);
	void advanceAnimation(AnimationComponent& animation);
};