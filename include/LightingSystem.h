#pragma once
#include "Chunk.h"
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>

struct LightNode { int worldX, y, level; };

class LightingSystem
{
public:
	LightingSystem();

	void UpdateLighting(const std::unordered_map<int, Chunk*>& renderedChunks);
	void UpdateLightingRegion(const std::unordered_map<int, Chunk*>& renderedChunks, int worldX, int worldY);

private:
	std::unordered_map<int, std::vector<std::vector<sf::Color>>> lightMaps;
	std::unordered_map<int, std::vector<std::vector<int>>> lightLevelMaps;

	const sf::Color WHITE_LIGHT = sf::Color::White;
	const sf::Color AMBIENT_LIGHT = sf::Color::Black;
	const float MAX_LIGHT_LEVEL = 15.0f;
	const int PLAYER_LIGHT_RADIUS = 12;

	void PropogateSunlight(const std::unordered_map<int, Chunk*>& renderedChunks);
	void RunBFS(const std::unordered_map<int, Chunk*>& renderedChunks, std::queue<LightNode> nodesToProcess);
	sf::Color InterpolateColor(float t) const;
};
