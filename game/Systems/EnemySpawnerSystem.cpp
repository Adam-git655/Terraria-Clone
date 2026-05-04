#include "EnemySpawnerSystem.h"

void EnemySpawnerSystem::update(EntityManager& mgr, EntityFactory& entityFactory, const std::unordered_map<Enemies, sf::Texture*>& enemyTextures, ChunksManager& chunksManager, Entt playerEntity, float dt)
{
	spawnTimer += dt;

	if (spawnTimer < spawnInterval)
		return;

	spawnTimer = 0.0f;

	//spawn enemy
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();
	Vec2& playerPos = transformStorage.get(playerEntity).position;

	Enemies enemy = pickRandomEnemy();

	if (enemy == Enemies::None)
		return;

	const sf::Texture* enemyTex = nullptr;

	auto it = enemyTextures.find(enemy);
	if (it != enemyTextures.end())
		enemyTex = it->second;

	if (enemyTex == nullptr)
		return;

	if (enemy == Enemies::Zombie)
	{
		Vec2 spawnPos = pickSpawnPosZombie(playerPos, chunksManager);
		entityFactory.createZombie(spawnPos, *enemyTex);
	}
	else if (enemy == Enemies::BloodBat)
	{
		Vec2 spawnPos = pickSpawnPosBloodBat(playerPos);
		entityFactory.createBloodBat(spawnPos, *enemyTex);
	}
}

Enemies EnemySpawnerSystem::pickRandomEnemy()
{
	std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
	
	float total = 0.0f;
	float chance = chanceDist(gen);

	for (const auto& enemy : enemySpawnChancesMap)
	{
		total += enemy.second;
		if (chance <= total)
			return enemy.first;
	}

	return Enemies::None; //Shouldnt happen as all chances add up to 1
}

Vec2 EnemySpawnerSystem::pickSpawnPosZombie(Vec2 playerPos, ChunksManager& chunksManager)
{
	std::uniform_int_distribution<int> spawnSideDist(0, 1);
	std::uniform_real_distribution<float> spawnOffsetDist(spawnRangeMin, spawnRangeMax);

	float spawnOffset = spawnOffsetDist(gen);
	if (spawnSideDist(gen) == 0)
		spawnOffset = -spawnOffset;

	float worldSpawnX = playerPos.x + spawnOffset;
	int chunkX = chunksManager.getChunkXFromWorldX(worldSpawnX);
	Chunk* chunk  = chunksManager.getChunkIfExists(chunkX);

	if (chunk == nullptr)
		return Vec2{ -1, -1 };

	int tileX = static_cast<int>(std::floor(worldSpawnX / Chunk::TILESIZE));
	int localX = tileX - chunkX * Chunk::CHUNK_WIDTH;

	int tileSpawnY = chunk->getSurfaceHeights()[localX] - 1;
	float worldSpawnY = (tileSpawnY + 0.5f) * Chunk::TILESIZE;

	return { worldSpawnX, worldSpawnY };
}

Vec2 EnemySpawnerSystem::pickSpawnPosBloodBat(Vec2 playerPos)
{
	std::uniform_int_distribution<int> spawnSideDist(0, 1);
	std::uniform_real_distribution<float> spawnOffsetDist(spawnRangeMin, spawnRangeMax);

	float spawnOffsetX = spawnOffsetDist(gen);
	if (spawnSideDist(gen) == 0)
		spawnOffsetX = -spawnOffsetX;

	float spawnOffsetY = spawnOffsetDist(gen);

	float worldSpawnX = playerPos.x + spawnOffsetX;
	float worldSpawnY = playerPos.y - spawnOffsetY;

	return { worldSpawnX, worldSpawnY };
}