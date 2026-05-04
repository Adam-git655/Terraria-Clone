#pragma once
#include "engine/core/Vec2.h"
#include "engine/world/ChunksManager.h"
#include "game/EntityFactory.h"

enum class Enemies
{
	Zombie,
	BloodBat,
	None
};

class EnemySpawnerSystem
{
public:
	void update(EntityManager& mgr, EntityFactory& entityFactory, const std::unordered_map<Enemies, sf::Texture*>& enemyTextures, ChunksManager& chunksManager, Entt playerEntity, float dt);

private:
	std::mt19937 gen{ std::random_device{}() };

	float spawnTimer = 0.0f;
	float spawnInterval = 3.0f;
	float spawnRangeMin = 200.0f;
	float spawnRangeMax = 1000.0f;

	std::vector<std::pair<Enemies, float>> enemySpawnChancesMap =
	{
		{Enemies::Zombie, 0.6},
		{Enemies::BloodBat, 0.4}
	};

	Enemies pickRandomEnemy();
	Vec2 pickSpawnPosZombie(Vec2 playerPos, ChunksManager& chunksManager);
	Vec2 pickSpawnPosBloodBat(Vec2 playerPos);
};