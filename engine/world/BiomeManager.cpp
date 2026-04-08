#include "BiomeManager.h"

BiomeManager::BiomeManager()
{
	//Initialize biome data
	biomeDataMap[BiomeType::Forest] = { 0.0035f, 0.7f, Tile::TileType::Grass, Tile::TileType::Dirt, 1, 5, 40, 0.4f, 0.5f, 0.3f, 0.7f, true, true };
	biomeDataMap[BiomeType::Desert] = { 0.0025f, 0.4f, Tile::TileType::Sand, Tile::TileType::SandStone, 6, 25, 30, 0.35f, 0.55f, 0.2f, 0.8f, false, true };

	//Add inital starting biome to always be forest
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> biomeSize(20, 40);

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
	std::uniform_int_distribution<int> biomeSize(20, 40);
	std::uniform_int_distribution<int> biome(0, 1);

	int newZoneStartX;
	int newZoneEndX;

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
	}

	biomeZones.push_back({ newZoneStartX, newZoneEndX, static_cast<BiomeType>(biome(gen)) });
}