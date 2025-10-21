#include "LightingSystem.h"

LightingSystem::LightingSystem()
{
	lightMap.resize(Chunk::CHUNK_WIDTH, std::vector<sf::Color>(Chunk::CHUNK_HEIGHT, WHITE_LIGHT));
	lightLevels.resize(Chunk::CHUNK_WIDTH, std::vector<int>(Chunk::CHUNK_HEIGHT, 0));
}

void LightingSystem::PropogateSunlight(Chunk& chunk)
{
	//Reset light levels to 0 throughout chunk
	for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
		{
			lightLevels[x][y] = 0;
		}
	}

	struct Node { int x, y, level; };

	std::queue<Node> nodesToProcess;

	//Intialize Sky region with max brightness

	for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		int surfaceY = chunk.getSurfaceHeights()[x];
		

		for (int y = 0; y < surfaceY; ++y)
		{
			Tile& tile = chunk.getTile(x, y);

			lightLevels[x][y] = (int)MAX_LIGHT_LEVEL;
			nodesToProcess.push({ x, y, (int)MAX_LIGHT_LEVEL });
		}
	}

	//Spread Lighting underground based on BFS Blood Fill algorithm
	const int dirs[4][2] =
	{
		{1,0}, {-1,0}, {0,1}, {0,-1}
	};

	while (!nodesToProcess.empty())
	{
		Node node = nodesToProcess.front();
		nodesToProcess.pop();

		for (auto& dir : dirs)
		{
			int newX = node.x + dir[0];
			int newY = node.y + dir[1];

			if (newX < 0 || newX >= Chunk::CHUNK_WIDTH || newY < 0 || newY >= Chunk::CHUNK_HEIGHT)
				continue;
			
			Tile& neighbour = chunk.getTile(newX, newY);
			int newLevel = node.level - 1;

			if (neighbour.isSolid())
			{
				newLevel -= 1;
			}
			if (newLevel > 0 && newLevel > lightLevels[newX][newY])
			{
				lightLevels[newX][newY] = newLevel;
				nodesToProcess.push({ newX, newY, newLevel });
			}
		}
	}

	//Turn light levels into real colors
	for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
		{
			float t = lightLevels[x][y] / 15.0f;
			lightMap[x][y] = InterpolateColor(t);
		}
	}

}

sf::Color LightingSystem::InterpolateColor(float t) const
{
	t = std::clamp(t, 0.0f, 1.0f);

	if (t > 0.8f)
	{
		return WHITE_LIGHT;
	}
	else
	{
		return sf::Color
		(
			static_cast<sf::Uint8>(AMBIENT_LIGHT.r + (WHITE_LIGHT.r - AMBIENT_LIGHT.r) * t * 1.25f),
			static_cast<sf::Uint8>(AMBIENT_LIGHT.g + (WHITE_LIGHT.g - AMBIENT_LIGHT.g) * t * 1.25f),
			static_cast<sf::Uint8>(AMBIENT_LIGHT.b + (WHITE_LIGHT.b - AMBIENT_LIGHT.b) * t * 1.25f)
		);
	}
}

void LightingSystem::UpdateLighting(Chunk& chunk)
{
	PropogateSunlight(chunk);

	for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
		{
			chunk.getTile(x, y).setLightColor(lightMap[x][y]);
		}
	}
}