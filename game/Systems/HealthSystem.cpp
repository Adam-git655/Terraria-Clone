#pragma once
#include "HealthSystem.h"

void HealthSystem::update(EntityManager& mgr)
{
	std::vector<Entt> dead;
	auto& healthStorage = mgr.getComponentStorage<HealthComponent>();
	auto& inputStorage = mgr.getComponentStorage<InputComponent>();

	for (auto& [e, health] : healthStorage.getAll())
	{
		if (health.health <= 0)
		{
			//if player
			if (inputStorage.has(e))
			{
				//reset player transform position
			}
			//mark entity as dead if not player
			else
			{
				dead.push_back(e);
			}
		}
	}

	for (Entt e : dead)
	{
		mgr.destroy(e);
	}
}