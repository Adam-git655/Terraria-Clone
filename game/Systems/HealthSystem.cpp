#pragma once
#include "HealthSystem.h"

void HealthSystem::update(EntityManager& mgr)
{
	auto& healthStorage = mgr.getComponentStorage<HealthComponent>();

	for (auto& [e, health] : healthStorage.getAll())
	{
		if (health.health <= 0)
		{
			//die
		}
	}
}