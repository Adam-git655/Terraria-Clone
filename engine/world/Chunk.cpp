#include "Chunk.h"
#include "ChunksManager.h"

Chunk::Chunk(int chunkX, int seed, ChunksManager* mgr, BiomeManager& biomeManager)
	:chunkX(chunkX), seed(seed), chunksManager(mgr), biomeManager(biomeManager)
{
	chunkTiles.resize(CHUNK_WIDTH, std::vector<Tile>(CHUNK_HEIGHT, { Tile::TileType::Air, false }));
    surfaceHeights.resize(CHUNK_WIDTH);

    biome = biomeManager.getBiomeAt(chunkX);
    biomeData = &biomeManager.getBiomeData(biome);
	generateTerrain();
}

void Chunk::generateTerrain()
{
    Perlin p(seed);

    //get height adjustment offset in new biome
    float offset = biomeManager.getZoneOffset(chunkX);

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        //convert the chunkTile x's into world x's
        int worldX = chunkX * CHUNK_WIDTH + x;

        float terrainHeight = biomeManager.computeHeight(worldX, biomeData->biomeFrequency, biomeData->biomeAmplitude, p);
        terrainHeight += offset;

        int intTerrainHeight = static_cast<int>(terrainHeight + 0.5f);
        surfaceHeights[x] = intTerrainHeight;

        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            //CAVE GEN
            float val = 0.0f;
            float totalAmp = 0.0f;
            float freq = 0.05f;
            float amp = 1.0f;

            for (int i = 0; i < 6; i++)
            {
                val += p.perlin2D(worldX * freq, y * freq) * amp;
                totalAmp += amp;

                freq *= 2.0f;
                amp *= 0.5f;
            }

            val /= totalAmp;
            val = (val + 1.0f) * 0.5f;
            val = (val - 0.5f) * 5.0 + 0.5f;

            val = std::clamp(val, 0.0f, 1.0f);

            //SET SURFACE TILES
            if (y < intTerrainHeight)
            {
                setTile(x, y, Tile::TileType::Air, false);
            }
            else if (y < intTerrainHeight + biomeData->surfaceTilePatchLength)
            {
                setTile(x, y, biomeData->surfaceTile, true);
            }
            else if (y < intTerrainHeight + biomeData->secondarySurfaceTilePatchLength)
            {
                setTile(x, y, biomeData->secondarySurfaceTile, true);
            }
            else if (y >= intTerrainHeight + biomeData->secondarySurfaceTilePatchLength && y < intTerrainHeight + biomeData->stoneTilePatchLength)
            {
                setTile(x, y, biomeData->stoneTile, true);
            }

            //Caves
            else if (y >= intTerrainHeight + biomeData->stoneTilePatchLength && y < intTerrainHeight + 50)
            {
                if (val > biomeData->caveThresholdMin && val < biomeData->caveThresholdMax)
                    setTile(x, y, Tile::TileType::CaveAir, false);
                else
                    setTile(x, y, biomeData->stoneTile, true);
            }
            else if (y >= intTerrainHeight + 50)
            {
                if (val > biomeData->deepCaveThresholdMin && val < biomeData->deepCaveThresholdMax)
                    setTile(x, y, Tile::TileType::CaveAir, false);
                else
                    setTile(x, y, biomeData->stoneTile, true);
            }
        }
    }

    if (biomeData->generateCaveEntrances)
        generateCaveEntrance();

    if (biomeData->generateTrees)

        generateTrees();

    randomZombieSpawn();
}

void Chunk::generateCaveEntrance()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> xDist(0, Chunk::CHUNK_WIDTH - 1);

    float caveEntranceChance = 0.2f;

    if (chanceDist(gen) < caveEntranceChance)
    {        
        int xInChunk = xDist(gen);

        int caveStartWorldX = chunkX * CHUNK_WIDTH + xInChunk;
        chunksManager->generateCaveEntrances(caveStartWorldX, surfaceHeights[xInChunk]);
    }
}

void Chunk::generateTrees()
{
    std::random_device rd;
    std::mt19937 gen((rd()));
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> xDist(0, CHUNK_WIDTH - 1);
    std::uniform_int_distribution<int> nTrees(1, 3);

    float treeSpawnChance = 0.6f;
    for (int i = 0; i < nTrees(gen); i++)
    {
        if (chanceDist(gen) < treeSpawnChance)
        {
            int treeXInChunk = xDist(gen);

            int treeWorldX = chunkX * CHUNK_WIDTH + treeXInChunk;
            int spawnY = surfaceHeights[treeXInChunk] - 1;

            if (chunksManager)
                chunksManager->QueueTreePosForGeneration(treeWorldX, spawnY);
        }
    }
}

void Chunk::randomZombieSpawn()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> xDist(0, CHUNK_WIDTH - 1);
    std::uniform_int_distribution<int> nZombies(1, 3);

    float zombieSpawnChance = 0.15f;
    for (int i = 0; i < nZombies(gen); i++)
    {
        if (chanceDist(gen) < zombieSpawnChance)
        {
            int spawnXInChunk = xDist(gen);
            int spawnWorldX = chunkX * CHUNK_WIDTH + spawnXInChunk;
            int spawnY = surfaceHeights[spawnXInChunk] - 1;
            
            float worldPosX = (spawnWorldX + 0.5f) * Chunk::TILESIZE;
            float worldPosY = (spawnY + 0.5f) * Chunk::TILESIZE;

            //TODO:- REPLACE WITH ENTITY FACTORY.CREATE ZOMBIE
            if (chunksManager)
            {         
                chunksManager->QueueZombieSpawn(worldPosX, worldPosY);
            }
        }
    }
}

void Chunk::updateSand()
{
    for (int x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (int y = CHUNK_HEIGHT - 2; y >= 0; --y) //Bottom to top to make sand fall every frame instead of all at once.
        {
            if (getTile(x, y).getType() == Tile::TileType::Sand)
            {
                sandFall(x, y);
            }
        }
    }
}

void Chunk::sandFall(int x, int y)
{
    if (y + 1 >= CHUNK_HEIGHT)
        return;
    if (getTile(x, y + 1).isSolid())
        return;

    setTile(x, y, Tile::TileType::Air, false);
    setTile(x, y + 1, Tile::TileType::Sand, true);
}

Tile& Chunk::getTile(int x, int y)
{
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT)
        return chunkTiles[x][y];
    std::cout << "ERROR: Attemting to get tile out of bounds\n";
    return Tile();
}

const Tile& Chunk::getTile(int x, int y) const
{
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT)
        return chunkTiles[x][y];
    std::cout << "ERROR: Attemting to get tile out of bounds\n";
    return Tile();
}

void Chunk::setTile(int x, int y, Tile::TileType type, bool solid)
{
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT)
    {
        chunkTiles[x][y].setType(type);
        chunkTiles[x][y].setSolid(solid);

		int worldX = chunkX * CHUNK_WIDTH + x;
        chunkTiles[x][y].setPosition(Vec2(worldX * TILESIZE, y * TILESIZE));
    }
    else
    {
        std::cout << "ERROR: Attempting to set tile out of bounds at (" << x << ", " << y << ")\n";
    }
}


std::vector<std::vector<Tile>>& Chunk::getChunkTiles() 
{
    return chunkTiles;
}

std::vector<int>& Chunk::getSurfaceHeights()
{
    return surfaceHeights;
}

int Chunk::getChunkX() const
{
    return chunkX;
}

BiomeType Chunk::getBiome() const
{
    return biome;
}

void Chunk::collisionsWithTerrain(CollisionComponent& collision, TransformComponent& transform, PhysicsComponent& physics, Entt e)
{
    //get bounds of entity in global x positions
    sf::FloatRect entityBounds = collision.bounds;
    Vec2 entityPrevPos = transform.prevPos;

    //std::cout << entityBounds.left << " " << entityBounds.top << " " << entityBounds.width << " " << entityBounds.height << "\n";
    
    // Make the collision bounds smaller of the entity
    int offsetX = 10;
    entityBounds.left += offsetX;
    entityBounds.width -= offsetX * 2;

    // Caluclate Chunk's world X range
    int chunkWorldStartX = chunkX * CHUNK_WIDTH * TILESIZE;
    int chunkWorldEndX = chunkWorldStartX + CHUNK_WIDTH * TILESIZE;

    //Exit if the entity is not inside the chunks worldX range
    if (entityBounds.left + entityBounds.width < chunkWorldStartX || entityBounds.left > chunkWorldEndX)
        return;

    //calculate the entity worldX bounds respective to the chunksStartWorldX
    int distEntityStartX = entityBounds.left - chunkWorldStartX;
    int distEntityEndX = (entityBounds.left + entityBounds.width) - chunkWorldStartX;

    //convert these into local tile indices (0-15)
    int startLocalX = std::max(0, static_cast<int>(distEntityStartX / TILESIZE));
    int endLocalX = std::min(CHUNK_WIDTH - 1, static_cast<int>(distEntityEndX / TILESIZE));
    //No need to convert y respective to chunks
    int startY = std::max(0, static_cast<int>(entityBounds.top / TILESIZE));
    int endY = std::min(CHUNK_HEIGHT - 1, static_cast<int>((entityBounds.top + entityBounds.height) / TILESIZE));

    if (startLocalX > endLocalX || startY > endY)
        return;

    // Check tiles in local coordinates
    for (int localX = startLocalX; localX <= endLocalX; ++localX) {
        for (int y = startY; y <= endY; ++y) {
            Tile& tile = chunkTiles[localX][y];
            if (tile.isSolid()) {
                sf::FloatRect tileBounds = tile.getBounds();

                if (entityBounds.intersects(tileBounds)) {

                    bool collisionFromLeft = entityPrevPos.x + entityBounds.width / 2 <= tileBounds.left;
                    bool collisionFromRight = entityPrevPos.x - entityBounds.width / 2 >= tileBounds.left + tileBounds.width;
                    bool collisionFromBottom = entityPrevPos.y - entityBounds.height / 2 >= tileBounds.top + tileBounds.height;
                    bool collisionFromTop = entityPrevPos.y + entityBounds.height / 2 <= tileBounds.top;

                    if (collisionFromLeft)
                    {
                        transform.position = Vec2(tileBounds.left - entityBounds.width / 2, transform.position.y);
                        physics.velocity = Vec2(0.0f, physics.velocity.y); // Stop rightward velocity
                    }

                    else if (collisionFromRight)
                    {
                        transform.position = Vec2(tileBounds.left + tileBounds.width + entityBounds.width / 2, transform.position.y);
                        physics.velocity = Vec2(0.0f, physics.velocity.y); // Stop leftwards velocity
                    }

                    else if (collisionFromBottom)
                    {
                        transform.position = Vec2(transform.position.x, tileBounds.top + tileBounds.height + entityBounds.height / 2);
                        physics.velocity = Vec2(physics.velocity.x, 0.0f); //Stop upwards vel
                    }

                    else if (collisionFromTop)
                    {
                        transform.position = Vec2(transform.position.x, tileBounds.top - entityBounds.height / 2);
                        physics.velocity = Vec2(physics.velocity.x, 0.0f); //Stop downwards vel
                    }
                }
            }
        }
    }
}