#pragma once
#include "Chunk.h"
#include "player.h"
#include <unordered_map>

class ChunksManager
{
private:
	std::unordered_map<int, std::unique_ptr<Chunk>> chunks;
	int seed;
	int renderDistance = 3;
	
	sf::Texture grassTex;
	sf::Texture dirtTex;
	sf::Texture stoneTex;

	int getChunkXFromWorldX(float worldX);

public:
	ChunksManager(int seed);

	Chunk& getChunk(int chunkX);

	void DestroyTile(sf::Vector2f pos);
	void PlaceTile(sf::Vector2f pos, Tile::TileType blockType);

	sf::Texture& getTexture(const std::string& textureName);

	void UpdateAndRenderChunks(Player& player, sf::RenderWindow& window); //Render chunks according to player position
	void playerCollision(Player& player);

};

