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
	if (!caveSkyTex.loadFromFile(RESOURCES_PATH "sky.png"))
	{
		std::cout << "ERROR LOADING CAVE SKY TEXTURE";
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
	if (!torchTex.loadFromFile(RESOURCES_PATH "torch.png"))
	{
		std::cout << "ERROR LOADING TORCH TEXTURE";
	}
	if (!sandTex.loadFromFile(RESOURCES_PATH "sand.png"))
	{
		std::cout << "ERROR LOADING SAND TEXTURE";
	}
	if (!sandStoneTex.loadFromFile(RESOURCES_PATH "sandstone.png"))
	{
		std::cout << "ERROR LOADING SANDSTONE TEXTURE";
	}
	if (!snowTex.loadFromFile(RESOURCES_PATH "snow.png"))
	{
		std::cout << "ERROR LOADING SNOW TEXTURE";
	}
	if (!iceTex.loadFromFile(RESOURCES_PATH "ice.png"))
	{
		std::cout << "ERROR LOADING ICE TEXTURE";
	}
	if (!snowLeafTex.loadFromFile(RESOURCES_PATH "snowLeaf.png"))
	{
		std::cout << "ERROR LOADING SNOWY LEAF TEXTURE";
	}
}

Chunk& ChunksManager::getChunk(int chunkX)
{
	if (chunks.find(chunkX) == chunks.end())
	{
		chunks[chunkX] = std::make_unique<Chunk>(chunkX, seed, this, biomeManager);
	}
	return *chunks[chunkX];
}

Chunk* ChunksManager::getChunkIfExists(int chunkX)
{
	auto it = chunks.find(chunkX);
	if (it == chunks.end())
	{
		return nullptr;
	}
	return it->second.get();
}

int ChunksManager::getChunkXFromWorldX(float worldX)
{
	int tileX = static_cast<int>(std::floor(worldX / Chunk::TILESIZE));

	int chunkX = tileX / Chunk::CHUNK_WIDTH;
	if (tileX < 0 && tileX % Chunk::CHUNK_WIDTH != 0)
		--chunkX;

	return chunkX;
}

Tile::TileType ChunksManager::DestroyTile(sf::Vector2f pos)
{

	int tileX = static_cast<int>(std::floor(pos.x / Chunk::TILESIZE));
	int tileY = static_cast<int>(std::floor(pos.y / Chunk::TILESIZE));

	int chunkX = getChunkXFromWorldX(pos.x);

	int localX = tileX - chunkX * Chunk::CHUNK_WIDTH;

	Chunk& chunk = getChunk(chunkX);
	Tile& tile = chunk.getTile(localX, tileY);
	Tile::TileType typeOfTileRemoved = tile.getType();

	if (tileY >= chunk.getSurfaceHeights()[localX])
		tile.setType(Tile::TileType::CaveAir);
	else
		tile.setType(Tile::TileType::Air);
	
	tile.setSolid(false);

	if (typeOfTileRemoved == Tile::TileType::Torch)
		lighting.RemoveLightSource(tileX, tileY);

	UpdateLightingForRegion(tileX, tileY);

	return typeOfTileRemoved;
}

bool ChunksManager::PlaceTile(sf::Vector2f pos, Tile::TileType blockType, bool solid)
{
	int tileX = static_cast<int>(std::floor(pos.x / Chunk::TILESIZE));
	int tileY = static_cast<int>(std::floor(pos.y / Chunk::TILESIZE));

	int chunkX = getChunkXFromWorldX(pos.x);

	int localX = tileX - chunkX * Chunk::CHUNK_WIDTH;

	Chunk& chunk = getChunk(chunkX);
	Tile& tile = chunk.getTile(localX, tileY);

	//cannot place another tile on top of torch tile or on top of same tile
	if (tile.getType() == Tile::TileType::Torch || tile.getType() == blockType)
	{
		//placement failed
		return false;
	}

	tile.setType(blockType);
	tile.setSolid(solid);

	if (blockType == Tile::TileType::Torch)
		lighting.AddLightSource(tileX, tileY, 15, sf::Color(255, 200, 150));

	UpdateLightingForRegion(tileX, tileY);

	//placement succesful
	return true;
}

const sf::Texture& ChunksManager::getTexture(const std::string& textureName) const
{
	if (textureName == "Grass")
		return grassTex;
	else if (textureName == "Dirt")
		return dirtTex;
	else if (textureName == "CaveAir")
		return caveSkyTex;
	else if (textureName == "Stone")
		return stoneTex;
	else if (textureName == "Wood")
		return woodTex;
	else if (textureName == "Leaf")
		return leafTex;
	else if (textureName == "Torch")
		return torchTex;
	else if (textureName == "Sand")
		return sandTex;
	else if (textureName == "SandStone")
		return sandStoneTex;
	else if (textureName == "Snow")
		return snowTex;
	else if (textureName == "Ice")
		return iceTex;
	else if (textureName == "SnowLeaf")
		return snowLeafTex;
	else
	{
		std::cerr << "ERROR: Unkown texture name provided in getTexture: " << textureName << "\n";
		return grassTex; //fallback
	}
}

void ChunksManager::UpdateAndRenderChunks(float dt, Vec2& playerPos, sf::RenderWindow& window)
{
	int playerChunkX = getChunkXFromWorldX(playerPos.x);

	//determine visible chunks based on their chunkX's (around the player)
	std::unordered_set<int> visibleChunkXs;
	visibleChunkXs.reserve((renderDistance * 2) + 1);

	for (int dx = -renderDistance; dx <= renderDistance; ++dx)
		visibleChunkXs.insert(playerChunkX + dx);
	
	//remove any chunks which are not in render distance anymore
	for (auto it = renderedChunks.begin(); it != renderedChunks.end();)
	{
		if (visibleChunkXs.find(it->first) == visibleChunkXs.end())
			it = renderedChunks.erase(it);
		else
			++it;
	} 

	//add any newly visible chunks
	for (int chunkX : visibleChunkXs)
	{
		if (renderedChunks.find(chunkX) == renderedChunks.end())
		{
			renderedChunks[chunkX] = &getChunk(chunkX);
			UpdateLighting();
		}
	}

	//Render the visible chunks
	for (int dx = -renderDistance; dx <= renderDistance; ++dx)
	{
		int currentChunkX = playerChunkX + dx;
		auto it = renderedChunks.find(currentChunkX);
		if (it == renderedChunks.end()) continue;

		Chunk& chunk = *it->second;

		//Update sand tiles in visible chunks
		chunk.updateSand();
		
		//Rendering the tiles
		for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
		{
			for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				Tile& tile = chunk.getTile(x, y);

				sf::Sprite tileSprite;

				switch (tile.getType())
				{
				case Tile::TileType::Air:
					//do nothing
					continue;

				case Tile::TileType::Grass:
					tileSprite.setTexture(grassTex);
					break;

				case Tile::TileType::Dirt:
					tileSprite.setTexture(dirtTex);
					break;

				case Tile::TileType::CaveAir:
					tileSprite.setTexture(caveSkyTex);
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

				case Tile::TileType::Torch:
					tileSprite.setTexture(torchTex);
					break;

				case Tile::TileType::Sand:
					tileSprite.setTexture(sandTex);
					break;

				case Tile::TileType::SandStone:
					tileSprite.setTexture(sandStoneTex);
					break;

				case Tile::TileType::Snow:
					tileSprite.setTexture(snowTex);
					break;

				case Tile::TileType::Ice:
					tileSprite.setTexture(iceTex);
					break;

				case Tile::TileType::SnowLeaf:
					tileSprite.setTexture(snowLeafTex);
					break;

				default:
					continue;
				}

				tileSprite.setColor(tile.getLightColor());

				int worldTileX = currentChunkX * Chunk::CHUNK_WIDTH + x;

				tileSprite.setPosition(worldTileX * Chunk::TILESIZE, y * Chunk::TILESIZE);

				window.draw(tileSprite);		
			}
		}
	}

	processTreeQueue();
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
					int tileY = y + oy;

					Chunk& chunk = getChunk(chunkX);
					
					if (tileY >= chunk.getSurfaceHeights()[localX])
						getChunk(chunkX).setTile(localX, tileY, Tile::TileType::CaveAir, false);
					else
						getChunk(chunkX).setTile(localX, tileY, Tile::TileType::Air, false);
				}
			}
		}

		dirX += ((rand() % 3) - 1) * 0.2f;

		dirY += ((rand() % 3) - 1) * 0.1f;
		if (dirY < 0.2f)
			dirY = 0.2f;

		x += (int)round(dirX);
		y += (int)round(dirY);

		int chunkX = floorDiv(x, Chunk::CHUNK_WIDTH);
		int localX = x - chunkX * Chunk::CHUNK_WIDTH;

		if (y < 50)
			break;
	}

	UpdateLighting();
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

	updateSnowLeaves();
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

	//Ensure that tree is not spawning in air
	if (chunk->getTile(localX, pos.y + 1).getType() == Tile::TileType::Air) return true;

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

void ChunksManager::updateSnowLeaves()
{
	for (auto& [chunkX, chunk] : renderedChunks)
	{
		//Set topmost leaves to snow if tundra biome
		if (chunk->getBiome() != BiomeType::Tundra)
			continue;

		for (int x = 0; x < Chunk::CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < Chunk::CHUNK_HEIGHT; y++)
			{
				//if already found snow leaf in that column, then break to next column
				if (chunk->getTile(x, y).getType() == Tile::TileType::SnowLeaf)
					break;

				if (chunk->getTile(x, y).getType() == Tile::TileType::Leaf)
				{
					//set first leaf from top to snow leaf in every column of chunk, and then break to the next column
					chunk->setTile(x, y, Tile::TileType::SnowLeaf, false);
					break;
				}
			}
		}
	}
}

void ChunksManager::DisableLighting()
{
	isLighting = false;
	for (auto& [chunkX, chunk] : renderedChunks)
	{
		for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
		{
			for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				chunk->getTile(x, y).setLightColor(sf::Color::White);
			}
		}
	}
}

void ChunksManager::EnableLighting()
{
	isLighting = true;
	UpdateLighting();
}

void ChunksManager::UpdateLighting()
{
	if (isLighting)
		lighting.UpdateLighting(renderedChunks);
}

void ChunksManager::UpdateLightingForRegion(int worldX, int worldY)
{
	if (isLighting)
		lighting.UpdateLightingRegion(renderedChunks, worldX, worldY);
}

void ChunksManager::QueueZombieSpawn(float worldX, float worldY)
{
	zombieSpawnPositions.push_back(Vec2(worldX, worldY));
}

void ChunksManager::QueueBloodBatSpawn(float worldX, float worldY)
{
	bloodBatSpawnPositions.push_back(Vec2(worldX, worldY));
}

std::vector<Vec2>& ChunksManager::getZombieSpawnPositions()
{
	return zombieSpawnPositions;
}

std::vector<Vec2>& ChunksManager::getBloodBatSpawnPositions()
{
	return bloodBatSpawnPositions;
}

void ChunksManager::collisionsWithTerrain(ComponentStorage<CollisionComponent>& collisionStorage,
										  ComponentStorage<TransformComponent>& transformStorage,
										  ComponentStorage<PhysicsComponent>& physicsStorage)
{
	for (auto& [e, collision] : collisionStorage.getAll())
	{
		auto& transform = transformStorage.get(e);
		auto& physics = physicsStorage.get(e);

		collision.bounds.left = transform.position.x - collision.bounds.width / 2.0f;
		collision.bounds.top = transform.position.y - collision.bounds.height / 2.0f;

		int chunkX = getChunkXFromWorldX(transform.position.x);
		Chunk& chunk = getChunk(chunkX);

		//Set isGrounded flag of entity
		int tileX = static_cast<int>(std::floor(transform.position.x / Chunk::TILESIZE));
		int tileBelow = static_cast<int>(std::floor(transform.position.y / Chunk::TILESIZE)) + 1;
		int localX = tileX - chunkX * Chunk::CHUNK_WIDTH;

		Tile& tile = chunk.getTile(localX, tileBelow);

		if (tile.isSolid())
			physics.IsOnGround = true;
		else
			physics.IsOnGround = false;
			

		chunk.collisionsWithTerrain(collision, transform, physics, e);	
	}
}