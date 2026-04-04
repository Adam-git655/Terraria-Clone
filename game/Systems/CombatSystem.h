#pragma once
#include "engine/ecs/EntityManager.h"
#include "game/Components/HealthComponent.h"
#include "game/Components/WeaponComponent.h"
#include "game/Components/TransformComponent.h"
#include "game/Components/FactionComponent.h"
#include "game/Components/AnimationComponent.h"
#include "game/Components/RenderComponent.h"

class CombatSystem
{
public:
	void update(EntityManager& mgr);
};