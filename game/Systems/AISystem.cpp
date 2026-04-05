#include "AISystem.h"

void AISystem::update(EntityManager& mgr, ChunksManager& chunksManager, Entt playerE, float dt)
{
	auto& aiStorage = mgr.getComponentStorage<AIComponent>();
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();
	auto& physicsStorage = mgr.getComponentStorage<PhysicsComponent>();
	auto& weaponStorage = mgr.getComponentStorage<WeaponComponent>();

	auto& playerTransform = transformStorage.get(playerE);
	
	for (auto& [e, ai] : aiStorage.getAll())
	{
		auto& transform = transformStorage.get(e);
		auto& physics = physicsStorage.get(e);

		float squaredDistanceToPlayer = transform.position.distSquared(playerTransform.position);
		ai.canSeePlayer =  squaredDistanceToPlayer <= (ai.visionRange * ai.visionRange);
		ai.canAttackPlayer = squaredDistanceToPlayer <= (ai.attackRange * ai.attackRange);

		if (ai.canAttackPlayer && weaponStorage.has(e))
		{
			auto& weapon = weaponStorage.get(e);
			weapon.attackRequested = true;
		}

		if (ai.canSeePlayer)
		{
			ai.pathRefreshTimer += dt;

			//Only recalculate path after some interval
			if (ai.pathRefreshTimer >= ai.pathRefreshInterval)
			{
				ai.pathRefreshTimer = 0.0f;

				int zombieGlobalTileX = static_cast<int>(std::floor(transform.position.x / Chunk::TILESIZE));
				int zombieGlobalTileY = static_cast<int>(std::floor(transform.position.y / Chunk::TILESIZE));
				int playerGlobalTileX = static_cast<int>(std::floor(playerTransform.position.x / Chunk::TILESIZE));
				int playerGlobalTileY = static_cast<int>(std::floor(playerTransform.position.y / Chunk::TILESIZE));

				ai.path = Pathfinding::solveAStar(zombieGlobalTileX, zombieGlobalTileY, playerGlobalTileX, playerGlobalTileY, ai.maxJumpHeight, chunksManager);
				ai.currentPathIndex = 1;

				ai.lines.setPrimitiveType(sf::LineStrip);
				ai.lines.resize(ai.path.size());

				for (size_t i = 0; i < ai.path.size(); ++i)
				{
					ai.lines[i].position = sf::Vector2f(ai.path[i].x * Chunk::TILESIZE + Chunk::TILESIZE / 2.0f, ai.path[i].y * Chunk::TILESIZE + Chunk::TILESIZE / 2.0f);
					ai.lines[i].color = sf::Color::Yellow;
				}
			}
			
			if (!ai.path.empty() && ai.currentPathIndex < ai.path.size())
			{
				ai.currentTile = ai.path[ai.currentPathIndex - 1];
				ai.nextTile = ai.path[ai.currentPathIndex];

				if (isAtTileCenter(transform.position, ai.nextTile))
				{
					ai.currentPathIndex++;
					if (ai.currentPathIndex >= ai.path.size())
					{
						physics.velocity.x = 0.0f;
						continue;
					}
					ai.nextTile = ai.path[ai.currentPathIndex];
				}
			}
		}
		else
		{
			physics.velocity.x = 0.0f;
		}
	}	
}

bool AISystem::isAtTileCenter(const Vec2& position, const IVec2& tilePos, const float tolerance)
{
	float tileCenterX = static_cast<float>(tilePos.x * Chunk::TILESIZE + Chunk::TILESIZE / 2.0f);
	float tileCenterY = static_cast<float>(tilePos.y * Chunk::TILESIZE + Chunk::TILESIZE / 2.0f);

	float dx = std::abs(position.x - tileCenterX);
	float dy = std::abs(position.y - tileCenterY);

	return dx <= tolerance && dy <= tolerance;
}