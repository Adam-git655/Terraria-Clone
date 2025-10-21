#pragma once
#include "Chunk.h"
#include "player.h"
#include "zombie.h"
#include "LightingSystem.h"
#include <unordered_map>

class ChunksManager
{
private:
	std::unordered_map<int, std::unique_ptr<Chunk>> chunks;
	std::vector<std::unique_ptr<Zombie>> zombies;
	std::vector<IVec2> treePositions;
	LightingSystem lighting;
	int seed;
	int renderDistance = 3;
	
	sf::Texture grassTex;
	sf::Texture dirtTex;
	sf::Texture caveSkyTex;
	sf::Texture stoneTex;
	sf::Texture woodTex;
	sf::Texture leafTex;

	Chunk& getChunk(int chunkX);
	int getChunkXFromWorldX(float worldX);

	void processTreeQueue();
	bool generateTree(const IVec2 pos);

public:
	ChunksManager(int seed);
	
	Chunk* getChunkIfExists(int chunkX);

	void DestroyTile(sf::Vector2f pos);
	void PlaceTile(sf::Vector2f pos, Tile::TileType blockType);

	const sf::Texture& getTexture(const std::string& textureName) const;

	const std::vector<std::unique_ptr<Zombie>>& getAllZombies() const;
	std::vector<std::unique_ptr<Zombie>>& ChunksManager::getAllZombies();

	void UpdateAndRenderChunks(float dt, Player& player, sf::RenderWindow& window); //Render chunks according to player position
	void collisionsWithTerrain(Entity& entity);

	void generateCaveEntrances(int startX, int startY);

	void spawnZombie(float spawnX, float spawnY);

	void UpdateLighting(Chunk& chunk);

	void QueueTreePosForGeneration(int x, int y);
};

