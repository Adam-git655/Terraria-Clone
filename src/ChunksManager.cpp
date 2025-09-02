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
	if (!woodTex.loadFromFile(RESOURCES_PATH "wood.png"))
	{
		std::cout << "ERROR LOADING WOOD TEXTURE";
	}
	if (!leafTex.loadFromFile(RESOURCES_PATH "leaf.png"))
	{
		std::cout << "ERROR LOADING LEAF TEXTURE";
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

Chunk* ChunksManager::getChunkIfExists(int chunkX)
{
	if (chunks.find(chunkX) == chunks.end())
	{
		return nullptr;
	}
	return chunks.find(chunkX)->second.get();
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
	else if (textureName == "Wood")
		return woodTex;
	else if (textureName == "Leaf")
		return leafTex;
	else if (textureName == "")
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

				case Tile::TileType::Wood:
					tileSprite.setTexture(woodTex);
					break;

				case Tile::TileType::Leaf:
					tileSprite.setTexture(leafTex);
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

	processTreeQueue();

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

void ChunksManager::generateCaveEntrances(int startX, int startY)
{
	auto floorDiv = [](int a, int b) { return (a >= 0) ? a / b : ((a + 1) / b) - 1; };

	int x = startX;
	int y = startY;
	float dirX = ((rand() % 3) - 1) * 0.4f;
	float dirY = 1;

	for (int step = 0; step < 40; step++)
	{
		int radius = (rand() % (5 - 2 + 1)) + 2;

		int carveOffsetX = (step < 10) ? ((rand() % 5) - 2) : ((rand() % 3) - 1);
		int centerX = x + carveOffsetX;

		for (int oy = -radius; oy <= radius; oy++)
		{	
			for (int ox = -radius; ox <= radius; ox++)
			{
				if (ox * ox + oy * oy <= radius * radius)
				{
					int chunkX = floorDiv(centerX + ox, Chunk::CHUNK_WIDTH);
					int localX = (centerX + ox) - chunkX * Chunk::CHUNK_WIDTH;
					getChunk(chunkX).setTile(localX, y + oy, Tile::TileType::Air, false);
				}
			}
		}

		dirX += ((rand() % 3) - 1) * 0.65f;

		dirY += ((rand() % 3) - 1) * 0.05f;
		if (dirY < 0.2f)
			dirY = 0.2f;

		x += (int)round(dirX);
		y += (int)round(dirY);

		int chunkX = floorDiv(x, Chunk::CHUNK_WIDTH);
		int localX = x - chunkX * Chunk::CHUNK_WIDTH;

		if (y < 50)
			break;
	}
}

void ChunksManager::QueueTreePosForGeneration(int x, int y)
{
	treePositions.push_back(IVec2(x, y));
}

void ChunksManager::processTreeQueue()
{
	auto it = treePositions.begin();
	while (it != treePositions.end())
	{
		if (generateTree(*it))
		{
			it = treePositions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

bool ChunksManager::generateTree(const IVec2 pos)
{
	auto floorDiv = [](int a, int b) { return (a >= 0) ? a / b : ((a + 1) / b) - 1; };

	//Generate random height for tree
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> heightDist(4, 12);

	int height = heightDist(gen);

	//Check if chunk exists for tree
	int chunkX = floorDiv(pos.x, Chunk::CHUNK_WIDTH);
	int localX = pos.x - chunkX * Chunk::CHUNK_WIDTH;

	Chunk* chunk = getChunkIfExists(chunkX);
	if (!chunk) return false;

	//Check if tile below is grass or not
	if (chunk->getTile(localX, pos.y + 1).getType() != Tile::TileType::Grass) return true;

	//Generate the trunk
	for (int i = 0; i < height; i++)
	{
		chunk->setTile(localX, pos.y - i, Tile::TileType::Wood, false);
	}

	//random canopy settings for tree
	std::uniform_int_distribution<int> canopyRadiusDist(2, 3);
	std::uniform_int_distribution<int> canopyShapeDist(1, 2);
	int canopyRadius = canopyRadiusDist(gen);
	int canopyShape = canopyShapeDist(gen);

	//Generate the canopy (diamond / circle shaped)
	for (int dx = -canopyRadius; dx <= canopyRadius; dx++)
	{
		for (int dy = -canopyRadius; dy <= canopyRadius; dy++)
		{
			bool shapeCondition = canopyShape == 1 ? abs(dx) + abs(dy) <= canopyRadius : dx * dx + dy * dy <= canopyRadius * canopyRadius;

			if (shapeCondition)
			{
				int chunkX = floorDiv(pos.x + dx, Chunk::CHUNK_WIDTH);
				int localX = (pos.x + dx) - chunkX * Chunk::CHUNK_WIDTH;

				Chunk* chunk = getChunkIfExists(chunkX);
				if (!chunk) return false;

				if (chunk->getTile(localX, pos.y - height + dy).getType() == Tile::TileType::Air)
					chunk->setTile(localX, pos.y - height + dy, Tile::TileType::Leaf, false);
			}
		}
	}

	return true;
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