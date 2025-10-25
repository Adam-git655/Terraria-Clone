#pragma once
#include "tile.h"
#include "PerlinNoise.h"
#include "Entity.h"
#include "random"
#include <vector>

class Zombie;
class ChunksManager;

class Chunk
{
private:
	int chunkX;
	std::vector<std::vector<Tile>> chunkTiles;
	std::vector<int> surfaceHeights;
	int seed;

	int lengthOfDirtPatch = 5;
	int lengthOfStonePatch = 40;

	void randomZombieSpawn();
	void generateCaveEntrance();
	void generateTrees();

	ChunksManager* chunksManager = nullptr;

public:
	static const int TILESIZE = 32;
	static const int CHUNK_WIDTH = 16;
	static const int CHUNK_HEIGHT = 512;

	Chunk(int chunkX, int seed, ChunksManager* mgr);

	void generateTerrain();
	void setTile(int localX, int localY, Tile::TileType type, bool solid);
	const Tile& getTile(int localX, int localY) const;
	Tile& getTile(int localX, int localY);
	int getChunkX() const;
	int getLengthOfDirtPatch() const;
	int getLengthOfStonePatch() const;

	std::vector<std::vector<Tile>>& getChunkTiles();
	std::vector<int>& getSurfaceHeights();

	void collisionsWithTerrain(Entity& entity);
};

