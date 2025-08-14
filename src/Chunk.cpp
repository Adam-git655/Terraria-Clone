#include "Chunk.h"
#include "zombie.h"

Chunk::Chunk(int chunkX, int seed, ChunksManager* mgr)
	:chunkX(chunkX), seed(seed), chunksManager(mgr)
{
	chunkTiles.resize(CHUNK_WIDTH, std::vector<Tile>(CHUNK_HEIGHT, { Tile::TileType::Air, false }));
	generateTerrain();
}

void Chunk::generateTerrain()
{
    Perlin p(seed);

    float frequency = 0.0035f;
    float amplitude = 0.7f;

    std::vector<int> surfaceHeights(CHUNK_WIDTH);

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {

        //convert the chunkTile x's into world x's
        int worldX = chunkX * CHUNK_WIDTH + x;

        float terrainHeight = 0.0f;

        float freq = frequency;
        float amp = amplitude;

        //6 octaves
        for (int i = 0; i < 6; i++)
        {
            terrainHeight += p.perlin(worldX * freq) * amp;

            freq *= 2.0f;
            amp *= 0.5f;
        }

        terrainHeight = (terrainHeight + 1.0f) * 0.5f * CHUNK_HEIGHT;

        int intTerrainHeight = static_cast<int>(terrainHeight + 0.5f);
        surfaceHeights[x] = intTerrainHeight;

        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            float val = 0.0f;
            float totalAmp = 0.0f;
            freq = 0.05f;
            amp = 1.0f;

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


            if (y < intTerrainHeight)
            {
                setTile(x, y, Tile::TileType::Air, false);
            }
            else if (y == intTerrainHeight)
            {
                setTile(x, y, Tile::TileType::Grass, true);
            }
            else if (y < intTerrainHeight + 5)
            {
                setTile(x, y, Tile::TileType::Dirt, true);
            }
            else if (y >= intTerrainHeight + 5 && y < intTerrainHeight + 40)
            {
                setTile(x, y, Tile::TileType::Stone, true);
            }
            else if (y >= intTerrainHeight + 40 && y < intTerrainHeight + 50)
            {
                if (val > 0.4f && val < 0.5f)
                    setTile(x, y, Tile::TileType::Air, false);
                else
                    setTile(x, y, Tile::TileType::Stone, true);
            }
            else if (y > intTerrainHeight + 50)
            {
                if (val > 0.3f && val < 0.7f)
                    setTile(x, y, Tile::TileType::Air, false);
                else
                    setTile(x, y, Tile::TileType::Stone, true);
            }
        }
    }

    generateCaveEntrance(surfaceHeights);
    randomZombieSpawn(surfaceHeights);
}

void Chunk::generateCaveEntrance(std::vector<int>& surfaceHeights)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);

    float caveEntranceChance = 0.3f;

    if (chanceDist(gen) < caveEntranceChance)
    {
        std::uniform_int_distribution<int> xDist(0, Chunk::CHUNK_WIDTH - 1);
        
        int xInChunk = xDist(gen);

        int caveStartWorldX = chunkX * CHUNK_WIDTH + xInChunk;
        chunksManager->generateCaveEntrances(caveStartWorldX, surfaceHeights[xInChunk]);
    }
}

void Chunk::randomZombieSpawn(std::vector<int>& surfaceHeights)
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
            
            float sfmlX = (spawnWorldX + 0.5f) * Chunk::TILESIZE;
            float sfmlY = (spawnY + 0.5f) * Chunk::TILESIZE;
            if (chunksManager)
            {         
                chunksManager->spawnZombie(sfmlX, sfmlY);
            }
        }
    }
}

Tile Chunk::getTile(int x, int y) const
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

void Chunk::collisionsWithTerrain(Entity& entity)
{
    //get bounds of player in global x positions
    sf::FloatRect entityBounds = entity.getSprite().getGlobalBounds();
    Vec2 entityPrevPos = entity.getPrevPos();

    // Make the collision bounds smaller of the player
    int offsetX = 10;
    entityBounds.left += offsetX;
    entityBounds.width -= offsetX * 2;

    // Caluclate Chunk's world X range
    int chunkWorldStartX = chunkX * CHUNK_WIDTH * TILESIZE;
    int chunkWorldEndX = chunkWorldStartX + CHUNK_WIDTH * TILESIZE;

    //Exit if the player is not inside the chunks worldX range
    if (entityBounds.left + entityBounds.width < chunkWorldStartX || entityBounds.left > chunkWorldEndX)
        return;

    //calculate the player worldX bounds respective to the chunksStartWorldX
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


    bool isGrounded = false;

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
                        entity.setPosition(Vec2(tileBounds.left - entityBounds.width / 2, entity.getPosition().y));
                        entity.setVelocity(Vec2(0.0f, entity.getVelocity().y)); // Stop rightward velocity
                    }

                    else if (collisionFromRight)
                    {
                        entity.setPosition(Vec2(tileBounds.left + tileBounds.width + entityBounds.width / 2, entity.getPosition().y));
                        entity.setVelocity(Vec2(0.0f, entity.getVelocity().y)); // Stop leftwards velocity
                    }

                    else if (collisionFromBottom)
                    {
                        entity.setPosition(Vec2(entity.getPosition().x, tileBounds.top + tileBounds.height + entityBounds.height / 2));
                        entity.setVelocity(Vec2(entity.getVelocity().x, 0.0f)); //Stop upwards vel
                    }

                    else if (collisionFromTop)
                    {
                        isGrounded = true;
                        entity.setPosition(Vec2(entity.getPosition().x, tileBounds.top - entityBounds.height / 2));
                        entity.setVelocity(Vec2(entity.getVelocity().x, 0.0f)); //Stop downwards vel
                    }
                }
            }
        }
    }
    entity.setIsOnGround(isGrounded);
}