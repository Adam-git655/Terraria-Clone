#include "Chunk.h"

Chunk::Chunk(int chunkX, int seed)
	:chunkX(chunkX), seed(seed)
{
	chunkTiles.resize(CHUNK_WIDTH, std::vector<Tile>(CHUNK_HEIGHT, { Tile::TileType::Air, false }));
	generateTerrain();
}

void Chunk::generateTerrain()
{
    Perlin p(seed);

    float frequency = 0.01f;
    float amplitude = 0.5f;

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

        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
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
            else
            {
                setTile(x, y, Tile::TileType::Stone, true);
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

void Chunk::playerCollisions(Player& player)
{
    //get bounds of player in global x positions
    sf::FloatRect playerBounds = player.getSprite().getGlobalBounds();
    Vec2 playerPrevPos = player.getPrevPos();

    // Make the collision bounds smaller of the player
    int offsetX = 10;
    playerBounds.left += offsetX;
    playerBounds.width -= offsetX * 2;

    // Caluclate Chunk's world X range
    int chunkWorldStartX = chunkX * CHUNK_WIDTH * TILESIZE;
    int chunkWorldEndX = chunkWorldStartX + CHUNK_WIDTH * TILESIZE;

    //Exit if the player is not inside the chunks worldX range
    if (playerBounds.left + playerBounds.width < chunkWorldStartX || playerBounds.left > chunkWorldEndX)
        return;

    //calculate the player worldX bounds respective to the chunksStartWorldX
    int distPlayerStartX = playerBounds.left - chunkWorldStartX;
    int distPlayerEndX = (playerBounds.left + playerBounds.width) - chunkWorldStartX;

    //convert these into local tile indices (0-15)
    int startLocalX = std::max(0, static_cast<int>(distPlayerStartX / TILESIZE));
    int endLocalX = std::min(CHUNK_WIDTH - 1, static_cast<int>(distPlayerEndX / TILESIZE));
    //No need to convert y respective to chunks
    int startY = std::max(0, static_cast<int>(playerBounds.top / TILESIZE));
    int endY = std::min(CHUNK_HEIGHT - 1, static_cast<int>((playerBounds.top + playerBounds.height) / TILESIZE));

    if (startLocalX > endLocalX || startY > endY)
        return;


    bool isGrounded = false;

    // Check tiles in local coordinates
    for (int localX = startLocalX; localX <= endLocalX; ++localX) {
        for (int y = startY; y <= endY; ++y) {
            Tile& tile = chunkTiles[localX][y];
            if (tile.isSolid()) {
                sf::FloatRect tileBounds = tile.getBounds();

                if (playerBounds.intersects(tileBounds)) {

                    bool collisionFromLeft = playerPrevPos.x + playerBounds.width / 2 <= tileBounds.left;
                    bool collisionFromRight = playerPrevPos.x - playerBounds.width / 2 >= tileBounds.left + tileBounds.width;
                    bool collisionFromBottom = playerPrevPos.y - playerBounds.height / 2 >= tileBounds.top + tileBounds.height;
                    bool collisionFromTop = playerPrevPos.y + playerBounds.height / 2 <= tileBounds.top;

                    if (collisionFromLeft)
                    {
                        player.setPosition(Vec2(tileBounds.left - playerBounds.width / 2, player.getSprite().getPosition().y));
                        player.setVelocity(Vec2(0.0f, player.getVelocity().y)); // Stop rightward velocity
                    }

                    else if (collisionFromRight)
                    {
                        player.setPosition(Vec2(tileBounds.left + tileBounds.width + playerBounds.width / 2, player.getSprite().getPosition().y));
                        player.setVelocity(Vec2(0.0f, player.getVelocity().y)); // Stop leftwards velocity
                    }

                    else if (collisionFromBottom)
                    {
                        player.setPosition(Vec2(player.getSprite().getPosition().x, tileBounds.top + tileBounds.height + playerBounds.height / 2));
                        player.setVelocity(Vec2(player.getVelocity().x, 0.0f)); //Stop upwards vel
                    }  
                    
                    else if (collisionFromTop)
                    {
                        isGrounded = true;
                        player.setPosition(Vec2(player.getSprite().getPosition().x, tileBounds.top - playerBounds.height / 2));
                        player.setVelocity(Vec2(player.getVelocity().x, 0.0f)); //Stop downwards vel
                    }
                }
            }
        }
    }
    player.setIsOnGround(isGrounded);
}