#pragma once
#include "tile.h"
#include "PerlinNoise.h"
#include "Player.h"
#include <vector>

class Chunk
{
private:
	int chunkX;
	std::vector<std::vector<Tile>> chunkTiles;
	int seed;

public:
	static const int TILESIZE = 32;
	static const int CHUNK_WIDTH = 16;
	static const int CHUNK_HEIGHT = 256;

	Chunk(int chunkX, int seed);

	void generateTerrain();
	void setTile(int localX, int localY, Tile::TileType type, bool solid);
	Tile getTile(int localX, int localY) const;

	std::vector<std::vector<Tile>>& getChunkTiles();

	void playerCollisions(Player& player);
};

