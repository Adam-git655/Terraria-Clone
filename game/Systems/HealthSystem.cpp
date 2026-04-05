#pragma once
#include "HealthSystem.h"

void HealthSystem::update(EntityManager& mgr, Vec2& playerSpawnPos)
{
	std::vector<Entt> dead;
	auto& healthStorage = mgr.getComponentStorage<HealthComponent>();
	auto& factionStorage = mgr.getComponentStorage<FactionComponent>();
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();

	for (auto& [e, health] : healthStorage.getAll())
	{
		if (health.health <= 0)
		{
			auto& faction = factionStorage.get(e);

			//If Player dead, then reset it back to spawn position
			if (faction.faction == Faction::Player)
			{
				health.health = health.maxHealth;

				auto& transform = transformStorage.get(e);
				transform.position = playerSpawnPos;
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