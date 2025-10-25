#include "LightingSystem.h"

LightingSystem::LightingSystem()
{
}

void LightingSystem::PropogateSunlight(std::unordered_map<int, Chunk*>& renderedChunks)
{
	for (const auto& [chunkX, chunkPtr] : renderedChunks)
	{
		if (!chunkPtr) continue;

		lightMaps[chunkX] = std::vector<std::vector<sf::Color>>(
			Chunk::CHUNK_WIDTH, std::vector<sf::Color>(Chunk::CHUNK_HEIGHT, WHITE_LIGHT));

		lightLevelMaps[chunkX] = std::vector<std::vector<int>>(
			Chunk::CHUNK_WIDTH, std::vector<int>(Chunk::CHUNK_HEIGHT, 0));
	}

	struct Node { int worldX, y, level; };

	std::queue<Node> nodesToProcess;

	auto floorDiv = [](int a, int b) { return (a >= 0) ? a / b : ((a + 1) / b) - 1; };

	//Intialize Sky region with max brightness
	for (const auto& [chunkX, chunkPtr] : renderedChunks)
	{
		if (!chunkPtr) continue;

		auto itLL = lightLevelMaps.find(chunkX);
		if (itLL == lightLevelMaps.end()) continue;

		auto& lightLevelMap = itLL->second;

		for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
		{
			int surfaceY = chunkPtr->getSurfaceHeights()[x];

			for (int y = 0; y < surfaceY; ++y)
			{
				if (x < 0 || x >= (int)lightLevelMap.size()) continue;
				if (y < 0 || y >= (int)lightLevelMap[x].size()) continue;

				lightLevelMap[x][y] = static_cast<int>(MAX_LIGHT_LEVEL);
				int worldX = chunkX * Chunk::CHUNK_WIDTH + x;
				nodesToProcess.push({ worldX, y, static_cast<int>(MAX_LIGHT_LEVEL) });
			}
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
			int newX = node.worldX + dir[0];
			int newY = node.y + dir[1];

			if (newY < 0 || newY >= Chunk::CHUNK_HEIGHT) continue;

			int chunkX = floorDiv(newX, Chunk::CHUNK_WIDTH);
			int localX = newX - chunkX * Chunk::CHUNK_WIDTH;

			if (localX < 0 || localX >= Chunk::CHUNK_WIDTH) continue;

			auto chunkIt = renderedChunks.find(chunkX);
			if (chunkIt == renderedChunks.end()) continue;

			Chunk* chunk = chunkIt->second;
			if (!chunk) continue;

			Tile& neighbour = chunk->getTile(localX, newY);

			int newLevel = node.level - 1;

			if (neighbour.isSolid())
			{
				newLevel -= 1;
			}

			auto itLL = lightLevelMaps.find(chunkX);
			if (itLL == lightLevelMaps.end()) continue;

			auto& lightLevelMap = itLL->second;

			if (localX < 0 || localX >= (int)lightLevelMap.size()) continue;
			if (newY < 0 || newY >= (int)lightLevelMap[localX].size()) continue;

			if (newLevel > 0 && newLevel > lightLevelMap[localX][newY])
			{
				lightLevelMap[localX][newY] = newLevel;
				nodesToProcess.push({ newX, newY, newLevel });
			}
		}
	}

	//Turn light levels into real colors
	for (const auto& [chunkX, chunkPtr] : renderedChunks)
	{
		if (!chunkPtr) continue;

		auto itLM = lightMaps.find(chunkX);
		auto itLL = lightLevelMaps.find(chunkX);
		if (itLM == lightMaps.end() || itLL == lightLevelMaps.end()) continue;

		auto& lightMap = itLM->second;
		auto& lightLevelMap = itLL->second;

		for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
		{
			for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				float t = static_cast<float>(lightLevelMap[x][y]) / 15.0f;
				lightMap[x][y] = InterpolateColor(t);
			}
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

void LightingSystem::UpdateLighting(std::unordered_map<int, Chunk*>& renderedChunks)
{
	if (renderedChunks.empty()) return;

	PropogateSunlight(renderedChunks);

	for (const auto& [chunkX, chunkPtr] : renderedChunks)
	{
		if (!chunkPtr) continue;

		auto itLM = lightMaps.find(chunkX);
		if (itLM == lightMaps.end()) continue;

		auto& lightMap = itLM->second;

		for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
		{
			for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				chunkPtr->getTile(x, y).setLightColor(lightMap[x][y]);
			}
		}
	}
}