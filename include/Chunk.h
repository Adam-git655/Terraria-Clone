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
	int seed;

	void randomZombieSpawn(std::vector<int>& surfaceHeights);
	void generateCaveEntrance(std::vector<int>& surfaceHeights);
	void generateTrees(std::vector<int>& surfaceHeights);

	ChunksManager* chunksManager = nullptr;

public:
	static const int TILESIZE = 32;
	static const int CHUNK_WIDTH = 16;
	static const int CHUNK_HEIGHT = 512;

	Chunk(int chunkX, int seed, ChunksManager* mgr);

	void generateTerrain();
	void setTile(int localX, int localY, Tile::TileType type, bool solid);
	Tile getTile(int localX, int localY) const;
	int getChunkX() const;

	std::vector<std::vector<Tile>>& getChunkTiles();

	void collisionsWithTerrain(Entity& entity);
};

