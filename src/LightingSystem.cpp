#include "LightingSystem.h"

LightingSystem::LightingSystem()
{
}

void LightingSystem::AddLightSource(int worldX, int worldY, int lightLevel, sf::Color color)
{
	lightSources.push_back({ worldX, worldY, lightLevel, color });
}

void LightingSystem::RemoveLightSource(int worldX, int worldY)
{
	lightSources.erase(
		std::remove_if(lightSources.begin(), lightSources.end(),
			[&](const LightSource& l)
			{
				return l.worldX == worldX && l.worldY == worldY;
			}),
		lightSources.end()
	);
}


void LightingSystem::UpdateLighting(const std::unordered_map<int, Chunk*>& renderedChunks)
{
	if (renderedChunks.empty()) return;

	PropogateSunlight(renderedChunks);

	UpdateLightSources(renderedChunks);

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

void LightingSystem::PropogateSunlight(const std::unordered_map<int, Chunk*>& renderedChunks)
{
	for (const auto& [chunkX, chunkPtr] : renderedChunks)
	{
		if (!chunkPtr) continue;

		lightMaps[chunkX] = std::vector<std::vector<sf::Color>>(
			Chunk::CHUNK_WIDTH, std::vector<sf::Color>(Chunk::CHUNK_HEIGHT, WHITE_LIGHT));

		lightLevelMaps[chunkX] = std::vector<std::vector<int>>(
			Chunk::CHUNK_WIDTH, std::vector<int>(Chunk::CHUNK_HEIGHT, 0));
	}

	std::queue<LightNode> nodesToProcess;

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

	RunBFS(renderedChunks, nodesToProcess);

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

void LightingSystem::UpdateLightSources(const std::unordered_map<int, Chunk*>& renderedChunks)
{
	auto floorDiv = [](int a, int b) { return (a >= 0) ? a / b : ((a + 1) / b) - 1; };

	std::queue<LightNode> LightNodesToProcess;

	for (const auto& source : lightSources)
	{
		int chunkX = floorDiv(source.worldX, Chunk::CHUNK_WIDTH);
		int localX = source.worldX - chunkX * Chunk::CHUNK_WIDTH;

		auto itLL = lightLevelMaps.find(chunkX);
		if (itLL == lightLevelMaps.end()) continue;

		auto& lightLevelMap = itLL->second;
		
		int newLevel = std::max(lightLevelMap[localX][source.worldY], source.lightLevel);
		lightLevelMap[localX][source.worldY] = newLevel;

		LightNodesToProcess.push({ source.worldX, source.worldY, source.lightLevel });
	}

	RunBFS(renderedChunks, LightNodesToProcess);

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


void LightingSystem::UpdateLightingRegion(const std::unordered_map<int, Chunk*>& renderedChunks, int worldX, int worldY)
{
	const int RADIUS = 10;

	auto floorDiv = [](int a, int b) { return (a >= 0) ? a / b : ((a + 1) / b) - 1; };

	//set all tiles in region to light level 0
	for (int dx = -RADIUS; dx <= RADIUS; ++dx)
	{
		for (int dy = -RADIUS; dy <= RADIUS; ++dy)
		{
			int x = worldX + dx;
			int y = worldY + dy;

			if (y < 0 || y >= Chunk::CHUNK_HEIGHT) continue;

			int chunkX = floorDiv(x, Chunk::CHUNK_WIDTH);
			int localX = x - chunkX * Chunk::CHUNK_WIDTH;

			auto chunkIt = renderedChunks.find(chunkX);
			if (chunkIt == renderedChunks.end()) continue;

			Chunk* chunk = chunkIt->second;

			auto itLL = lightLevelMaps.find(chunkX);
			if (itLL == lightLevelMaps.end()) continue;

			auto& lightLevelMap = itLL->second;

			lightLevelMap[localX][y] = 0;
		}
	}

	std::queue<LightNode> nodesToProcess;

	//Push the nodes within a radius that are affected by sunlight
	for (int dx = -RADIUS; dx <= RADIUS; ++dx)
	{
		for (int dy = -RADIUS; dy <= RADIUS; ++dy)
		{
			int x = worldX + dx;
			int y = worldY + dy;

			if (y < 0 || y >= Chunk::CHUNK_HEIGHT) continue;

			int chunkX = floorDiv(x, Chunk::CHUNK_WIDTH);
			int localX = x - chunkX * Chunk::CHUNK_WIDTH;

			auto chunkIt = renderedChunks.find(chunkX);
			if (chunkIt == renderedChunks.end()) continue;

			Chunk* chunk = chunkIt->second;

			if (y < chunk->getSurfaceHeights()[localX])
			{
				nodesToProcess.push({ x, y, static_cast<int>(MAX_LIGHT_LEVEL) });
			}
		}
	}

	//Push Border tiles just outside the radius that are affected by any light
	const int BORDER = 1;
	for (int dx = -RADIUS - BORDER; dx <= RADIUS + BORDER; ++dx)
	{
		for (int dy = -RADIUS - BORDER; dy <= RADIUS + BORDER; ++dy)
		{
			if (std::abs(dx) <= RADIUS && std::abs(dy) <= RADIUS)
				continue;

			int x = worldX + dx;
			int y = worldY + dy;

			if (y < 0 || y >= Chunk::CHUNK_HEIGHT) continue;

			int chunkX = floorDiv(x, Chunk::CHUNK_WIDTH);
			int localX = x - chunkX * Chunk::CHUNK_WIDTH;

			auto chunkIt = renderedChunks.find(chunkX);
			if (chunkIt == renderedChunks.end()) continue;

			Chunk* chunk = chunkIt->second;

			auto itLL = lightLevelMaps.find(chunkX);
			if (itLL == lightLevelMaps.end()) continue;
			auto& lightLevelMap = itLL->second;

			if (localX < 0 || localX >= (int)lightLevelMap.size()) continue;

			int lightLevel = lightLevelMap[localX][y];
			if (lightLevel > 0)
			{
				nodesToProcess.push({ x, y, lightLevel });
			}
		}
	}

	//Include light sources in the region
	for (const auto& src : lightSources)
	{
		if (std::abs(src.worldX - worldX) <= RADIUS && std::abs(src.worldY - worldY) <= RADIUS)
		{
			nodesToProcess.push({ src.worldX, src.worldY, src.lightLevel });
		}
	}

	RunBFS(renderedChunks, nodesToProcess);

	//Apply final colors in affected region
	for (int dx = -RADIUS; dx <= RADIUS; ++dx)
	{
		for (int dy = -RADIUS; dy <= RADIUS; ++dy)
		{
			int x = worldX + dx;
			int y = worldY + dy;
			if (y < 0 || y >= Chunk::CHUNK_HEIGHT) continue;

			int chunkX = floorDiv(x, Chunk::CHUNK_WIDTH);
			int localX = x - chunkX * Chunk::CHUNK_WIDTH;

			auto chunkIt = renderedChunks.find(chunkX);
			if (chunkIt == renderedChunks.end()) continue;
			Chunk* chunk = chunkIt->second;

			auto itLM = lightMaps.find(chunkX);
			auto itLL = lightLevelMaps.find(chunkX);
			if (itLM == lightMaps.end() || itLL == lightLevelMaps.end()) continue;

			auto& lightMap = itLM->second;
			auto& lightLevelMap = itLL->second;

			float t = static_cast<float>(lightLevelMap[localX][y]) / 15.0f;
			lightMap[localX][y] = InterpolateColor(t);

			chunk->getTile(localX, y).setLightColor(lightMap[localX][y]);
		}
	}
}

void LightingSystem::RunBFS(const std::unordered_map<int, Chunk*>& renderedChunks, std::queue<LightNode> nodesToProcess)
{
	auto floorDiv = [](int a, int b) { return (a >= 0) ? a / b : ((a + 1) / b) - 1; };

	//Spread Lighting underground based on BFS Blood Fill algorithm
	const int dirs[4][2] =
	{
		{1,0}, {-1,0}, {0,1}, {0,-1}
	};

	while (!nodesToProcess.empty())
	{
		LightNode node = nodesToProcess.front();
		nodesToProcess.pop();

		for (auto& dir : dirs)
		{
			int newX = node.worldX + dir[0];
			int newY = node.y + dir[1];

			if (newY < 0 || newY >= Chunk::CHUNK_HEIGHT) continue;

			int chunkX = floorDiv(newX, Chunk::CHUNK_WIDTH);
			int localX = newX - chunkX * Chunk::CHUNK_WIDTH;

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
