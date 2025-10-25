#pragma once
#include "Chunk.h"
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>

class LightingSystem
{
private:
	std::unordered_map<int, std::vector<std::vector<sf::Color>>> lightMaps;
	std::unordered_map<int, std::vector<std::vector<int>>> lightLevelMaps;

	const sf::Color WHITE_LIGHT = sf::Color::White;
	const sf::Color AMBIENT_LIGHT = sf::Color::Black;
	const float MAX_LIGHT_LEVEL = 15.0f;
	const int PLAYER_LIGHT_RADIUS = 12;

public:
	LightingSystem();

	void UpdateLighting(std::unordered_map<int, Chunk*>& renderedChunks);

private:
	void PropogateSunlight(std::unordered_map<int, Chunk*>& renderedChunks);
	sf::Color InterpolateColor(float t) const;

};
