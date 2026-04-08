#pragma once
#include "tile.h"
#include <random>

enum class BiomeType
{
	Forest = 0,
	Desert
};

struct BiomeData
{
	float biomeFrequency = 0.0035f;
	float biomeAmplitude = 0.7f;

	Tile::TileType surfaceTile = Tile::TileType::Grass;
	Tile::TileType secondarySurfaceTile = Tile::TileType::Dirt;

	int surfaceTilePatchLength = 1;
	int secondarySurfaceTilePatchLength = 5;
	int stoneTilePatchLength = 40;

	float caveThresholdMin = 0.4f;
	float caveThresholdMax = 0.5f;
	float deepCaveThresholdMin = 0.3f;
	float deepCaveThresholdMax = 0.7f;

	bool generateTrees = true;
	bool generateCaveEntrances = true;
};

struct BiomeZone
{
	int startChunkX;
	int endChunkX;
	BiomeType biome;
};

class BiomeManager
{
public:
	BiomeManager();

	BiomeType getBiomeAt(int chunkX);
	const BiomeData& getBiomeData(BiomeType biome);

private:
	std::vector<BiomeZone> biomeZones;
	std::unordered_map<BiomeType, BiomeData> biomeDataMap;
	void createBiomeZone(int chunkX);
};