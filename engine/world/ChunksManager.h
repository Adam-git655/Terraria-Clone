#pragma once
#include "Chunk.h"
#include "engine/rendering/LightingSystem.h"
#include "BiomeManager.h"
#include <unordered_map>
#include <unordered_set>

class ChunksManager
{
public:
	ChunksManager(int seed);
	
	Chunk* getChunkIfExists(int chunkX);

	Tile::TileType DestroyTile(sf::Vector2f pos);
	bool PlaceTile(sf::Vector2f pos, Tile::TileType blockType, bool solid);

	const sf::Texture& getTexture(const std::string& textureName) const;

	void UpdateAndRenderChunks(float dt, Vec2& playerPos, sf::RenderWindow& window); //Render chunks according to player position
	void collisionsWithTerrain(ComponentStorage<CollisionComponent>& collisionStorage,
							   ComponentStorage<TransformComponent>& transformStorage,
		                       ComponentStorage<PhysicsComponent>& physicsStorage);

	void generateCaveEntrances(int startX, int startY);

	void DisableLighting();
	void EnableLighting();
	void UpdateLighting();
	void UpdateLightingForRegion(int worldX, int worldY);

	void QueueTreePosForGeneration(int x, int y);

	void QueueZombieSpawn(float worldX, float worldY);
	std::vector<Vec2>& getZombieSpawnPositions();

private:
	std::unordered_map<int, std::unique_ptr<Chunk>> chunks;
	std::unordered_map<int, Chunk*> renderedChunks;
	std::vector<IVec2> treePositions;
	std::vector<Vec2> zombieSpawnPositions;

	LightingSystem lighting;
	int seed;
	int renderDistance = 3;
	bool isLighting = true;

	BiomeManager biomeManager{ seed };

	sf::Texture grassTex;
	sf::Texture dirtTex;
	sf::Texture caveSkyTex;
	sf::Texture stoneTex;
	sf::Texture woodTex;
	sf::Texture leafTex;
	sf::Texture torchTex;
	sf::Texture sandTex;
	sf::Texture sandStoneTex;
	sf::Texture snowTex;
	sf::Texture iceTex;
	sf::Texture snowLeafTex;

	Chunk& getChunk(int chunkX);
	int getChunkXFromWorldX(float worldX);

	void processTreeQueue();
	bool generateTree(const IVec2 pos);
	void updateSnowLeaves();
};

