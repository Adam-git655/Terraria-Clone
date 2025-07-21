#include "ChunksManager.h"

ChunksManager::ChunksManager(int seed)
	:seed(seed)
{
	if (!grassTex.loadFromFile(RESOURCES_PATH "grass.png"))
	{
		std::cout << "ERROR LOADING GRASS TEXTURE";
	}
	if (!dirtTex.loadFromFile(RESOURCES_PATH "dirt.png"))
	{
		std::cout << "ERROR LOADING DIRT TEXTURE";
	}
	if (!stoneTex.loadFromFile(RESOURCES_PATH "stone.png"))
	{
		std::cout << "ERROR LOADING STONE TEXTURE";
	}
}

Chunk& ChunksManager::getChunk(int chunkX)
{
	if (chunks.find(chunkX) == chunks.end())
	{
		chunks[chunkX] = std::make_unique<Chunk>(chunkX, seed, this);
	}
	return *chunks[chunkX];
}

int ChunksManager::getChunkXFromWorldX(float worldX)
{
	int tileX = static_cast<int>(std::floor(worldX / Chunk::TILESIZE));

	int chunkX = tileX / Chunk::CHUNK_WIDTH;
	if (tileX < 0 && tileX % Chunk::CHUNK_WIDTH != 0)
		--chunkX;

	return chunkX;
}

void ChunksManager::DestroyTile(sf::Vector2f pos)
{

	int tileX = static_cast<int>(std::floor(pos.x / Chunk::TILESIZE));
	int tileY = static_cast<int>(std::floor(pos.y / Chunk::TILESIZE));

	int chunkX = getChunkXFromWorldX(pos.x);

	int localX = tileX - chunkX * Chunk::CHUNK_WIDTH;

	Chunk& chunk = getChunk(chunkX);
	auto& chunkTiles = chunk.getChunkTiles();

	chunkTiles[localX][tileY].setType(Tile::TileType::Air);
	chunkTiles[localX][tileY].setSolid(false);
}

void ChunksManager::PlaceTile(sf::Vector2f pos, Tile::TileType blockType)
{
	int tileX = static_cast<int>(std::floor(pos.x / Chunk::TILESIZE));
	int tileY = static_cast<int>(std::floor(pos.y / Chunk::TILESIZE));

	int chunkX = getChunkXFromWorldX(pos.x);

	int localX = tileX - chunkX * Chunk::CHUNK_WIDTH;

	Chunk& chunk = getChunk(chunkX);
	auto& chunkTiles = chunk.getChunkTiles();

	chunkTiles[localX][tileY].setType(blockType);
	chunkTiles[localX][tileY].setSolid(true);
}

const sf::Texture& ChunksManager::getTexture(const std::string& textureName) const
{
	if (textureName == "Grass")
		return grassTex;
	else if (textureName == "Dirt")
		return dirtTex;
	else if (textureName == "Stone")
		return stoneTex;
	std::cout << "ERROR\n";
}

const std::vector<std::unique_ptr<Zombie>>& ChunksManager::getAllZombies() const
{
	return zombies;
}

std::vector<std::unique_ptr<Zombie>>& ChunksManager::getAllZombies()
{
	return zombies;
}

void ChunksManager::UpdateAndRenderChunks(float dt, Player& player, sf::RenderWindow& window)
{
	sf::Sprite tileSprite;

	int chunkX = getChunkXFromWorldX(player.getSprite().getPosition().x);

	for (int dx = -renderDistance; dx <= renderDistance; ++dx)
	{
		int currentChunkX = chunkX + dx;
		Chunk& chunk = getChunk(currentChunkX);

		//Rendering the tiles
		for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
		{
			for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				Tile tile = chunk.getTile(x, y);

				switch (tile.getType())
				{
				case Tile::TileType::Air:
					//render nothing in case of air
					continue;

				case Tile::TileType::Grass:
					tileSprite.setTexture(grassTex);
					break;

				case Tile::TileType::Dirt:
					tileSprite.setTexture(dirtTex);
					break;

				case Tile::TileType::Stone:
					tileSprite.setTexture(stoneTex);
					break;

				default:
					continue;
				}

				int worldTileX = currentChunkX * Chunk::CHUNK_WIDTH + x;

				tileSprite.setPosition(worldTileX * Chunk::TILESIZE, y * Chunk::TILESIZE);

				window.draw(tileSprite);		
			}
		}
	}

	for (const auto& zombie : zombies)
	{
		if (zombie)
		{
			zombie->update(dt, player, *this, window);
			window.draw(zombie->getSprite());
		}
	}

	zombies.erase(std::remove_if(zombies.begin(), zombies.end(),
		[](const std::unique_ptr<Zombie>& zombie)
		{ return !zombie->isAlive(); }),
		zombies.end());
}

void ChunksManager::spawnZombie(float spawnX, float spawnY)
{
	zombies.push_back(std::make_unique<Zombie>(Vec2(spawnX, spawnY)));
}

void ChunksManager::collisionsWithTerrain(Entity& entity)
{
	for (auto& pair : chunks)
	{
		Chunk& chunk = *(pair.second);
		chunk.collisionsWithTerrain(entity);
	}
}