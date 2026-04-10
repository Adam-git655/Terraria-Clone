#include "BiomeManager.h"
#include "Chunk.h"

BiomeManager::BiomeManager(int seed)
	:seed(seed)
{
	//Initialize biome data
	biomeDataMap[BiomeType::Forest] = { 0.0035f, 0.7f, Tile::TileType::Grass, Tile::TileType::Dirt, 1, 5, 40, 0.4f, 0.5f, 0.3f, 0.7f, true, true };
	biomeDataMap[BiomeType::Desert] = { 0.0025f, 0.4f, Tile::TileType::Sand, Tile::TileType::SandStone, 6, 25, 30, 0.35f, 0.55f, 0.2f, 0.8f, false, true };

	//Add inital starting biome to always be forest
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> biomeSize(minBiomeSize, maxBiomeSize);

	int size = biomeSize(gen);
	biomeZones.push_back({ -size / 2, size / 2, BiomeType::Forest });
}

BiomeType BiomeManager::getBiomeAt(int chunkX)
{
	for (auto& zone : biomeZones)
	{
		if (chunkX >= zone.startChunkX && chunkX <= zone.endChunkX)
			return zone.biome;
	}

	createBiomeZone(chunkX);
	return getBiomeAt(chunkX);
}

const BiomeData& BiomeManager::getBiomeData(BiomeType biome)
{
	return biomeDataMap.at(biome);
}

void BiomeManager::createBiomeZone(int chunkX)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> biomeSize(minBiomeSize, maxBiomeSize);
	std::uniform_int_distribution<int> biome(0, 1);

	int newZoneStartX;
	int newZoneEndX;
	int biomeBoundaryWorldX;
	int prevBiomeChunkX;

	if (chunkX < 0)
	{
		int nearestZoneStartX = 0;
		for (auto& zone : biomeZones)
		{
			if (zone.startChunkX < nearestZoneStartX)
				nearestZoneStartX = zone.startChunkX;
		}

		newZoneEndX = nearestZoneStartX - 1;
		newZoneStartX = newZoneEndX - biomeSize(gen);

		//Tile right of boundary. i.e last tile of old biome
		biomeBoundaryWorldX = (newZoneEndX + 1) * Chunk::CHUNK_WIDTH;
		prevBiomeChunkX = newZoneEndX + 1;
	}
	else
	{
		int nearestZoneEndX = 0;
		for (auto& zone : biomeZones)
		{
			if (zone.endChunkX > nearestZoneEndX)
				nearestZoneEndX = zone.endChunkX;
		}

		newZoneStartX = nearestZoneEndX + 1;
		newZoneEndX = newZoneStartX + biomeSize(gen);

		//Tile left of boundary. i.e last tile of old biome
		biomeBoundaryWorldX = newZoneStartX * Chunk::CHUNK_WIDTH;
		prevBiomeChunkX = newZoneStartX - 1;
	}


	//Biome type selection (ensure alternating of biomes, since only two are there right now)
	BiomeType biomeToSpawn;

	if (lastSpawnedBiome == BiomeType::Desert)
	{
		biomeToSpawn = BiomeType::Forest;
		lastSpawnedBiome = BiomeType::Forest;
	}
	else
	{
		biomeToSpawn = BiomeType::Desert;
		lastSpawnedBiome = BiomeType::Desert;
	}

	//Compute height offset between two biomes
	BiomeZone newZone = { newZoneStartX, newZoneEndX, biomeToSpawn , 0.0f };

	Perlin p(seed);
	BiomeType prevBiome = getBiomeAt(prevBiomeChunkX);
	
	float hPrev = computeHeight(biomeBoundaryWorldX, getBiomeData(prevBiome).biomeFrequency, getBiomeData(prevBiome).biomeAmplitude, p);
	float hNew = computeHeight(biomeBoundaryWorldX, getBiomeData(biomeToSpawn).biomeFrequency, getBiomeData(biomeToSpawn).biomeAmplitude, p);

	//Compound height offset from previous biomes
	float prevOffset = getZoneOffset(prevBiomeChunkX);
	newZone.heightOffset = prevOffset + (hPrev - hNew);


	biomeZones.push_back(newZone);
}

float BiomeManager::computeHeight(int worldX, float startFreq, float startAmp, Perlin& p)
{
	float height = 0.0f;
	float freq = startFreq;
	float amp = startAmp;

	for (int i = 0; i < 6; i++)
	{
		height += p.perlin(worldX * freq) * amp;

		freq *= 2.0f;
		amp *= 0.5f;
	}

	return (height + 1.0f) * 0.5f * Chunk::CHUNK_HEIGHT;
}

float BiomeManager::getZoneOffset(int chunkX)
{
	for (auto& zone : biomeZones)
	{
		if (chunkX >= zone.startChunkX && chunkX <= zone.endChunkX)
			return zone.heightOffset;
	}
	return 0.0f;
}