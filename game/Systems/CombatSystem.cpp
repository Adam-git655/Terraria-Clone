#include "CombatSystem.h"

void CombatSystem::update(EntityManager& mgr)
{
	auto& healthStorage = mgr.getComponentStorage<HealthComponent>();
	auto& weaponStorage = mgr.getComponentStorage<WeaponComponent>();
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();
	auto& factionStorage = mgr.getComponentStorage<FactionComponent>();
	auto& animationStorage = mgr.getComponentStorage<AnimationComponent>();
	auto& renderStorage = mgr.getComponentStorage<RenderComponent>();

	for (auto& [attackerE, weapon] : weaponStorage.getAll())
	{
		if (!weapon.attackRequested)
			continue;

		if (weapon.attackClock.getElapsedTime().asSeconds() < weapon.attackCooldown)
			continue;

		weapon.attackRequested = false;

		auto& attackerTransform = transformStorage.get(attackerE);
		auto& attackerFaction = factionStorage.get(attackerE);

		if (attackerFaction.faction == Faction::Player)
			animationStorage.get(attackerE).play("swing", true);

		for (auto& [targetE, health] : healthStorage.getAll())
		{
			if (targetE == attackerE)
				continue;

			if (factionStorage.has(targetE))
			{
				auto& targetFaction = factionStorage.get(targetE);

				if (targetFaction.faction == attackerFaction.faction)
					continue;
			}

			//apply damage to target entities within attack range

			auto& targetTransform = transformStorage.get(targetE);

			if (renderStorage.get(attackerE).facingRight && targetTransform.position.x < attackerTransform.position.x)
				continue;
			if (!renderStorage.get(attackerE).facingRight && targetTransform.position.x > attackerTransform.position.x)
				continue;

			float squaredDist = attackerTransform.position.distSquared(targetTransform.position);

			if (squaredDist <= weapon.attackRange * weapon.attackRange)
			{
				health.health -= weapon.damage;
			}
		}

		weapon.attackClock.restart();
	}
}