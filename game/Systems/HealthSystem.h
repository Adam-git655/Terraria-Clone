#pragma once
#include "engine/ecs/EntityManager.h"
#include "game/Components/HealthComponent.h"
#include "game/Components/InputComponent.h"

class HealthSystem
{
public:
	void update(EntityManager& mgr);
};