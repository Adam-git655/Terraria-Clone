#pragma once
#include "tile.h"
#include "PerlinNoise.h"
#include <random>

enum class BiomeType
{
	Forest = 0,
	Desert = 1,
	Tundra = 2
};

struct BiomeData
{
	float biomeFrequency = 0.0035f;
	float biomeAmplitude = 0.7f;

	Tile::TileType surfaceTile = Tile::TileType::Grass;
	Tile::TileType secondarySurfaceTile = Tile::TileType::Dirt;
	Tile::TileType stoneTile = Tile::TileType::Stone;

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
	float heightOffset = 0.0f;
};

class BiomeManager
{
public:
	BiomeManager(int seed);

	BiomeType getBiomeAt(int chunkX);
	const BiomeData& getBiomeData(BiomeType biome);
	float computeHeight(int worldX, float startFreq, float startAmp, Perlin& p);
	float getZoneOffset(int chunkX);

private:
	std::vector<BiomeZone> biomeZones;
	std::unordered_map<BiomeType, BiomeData> biomeDataMap;
	int BLEND_WIDTH = 5;
	int seed;
	int minBiomeSize = 10;
	int maxBiomeSize = 20;

	BiomeType lastSpawnedBiome = BiomeType::Forest;

	void createBiomeZone(int chunkX);
};