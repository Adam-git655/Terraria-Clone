#pragma once
#include "Chunk.h"
#include <queue>
#include <SFML/Graphics.hpp>

class Chunk;

class LightingSystem
{
private:
	std::vector<std::vector<sf::Color>> lightMap;
	std::vector<std::vector<int>> lightLevels;

	const sf::Color WHITE_LIGHT = sf::Color::White;
	const sf::Color AMBIENT_LIGHT = sf::Color::Black;
	const float MAX_LIGHT_LEVEL = 15.0f;
	const int PLAYER_LIGHT_RADIUS = 12;

public:
	LightingSystem();

	void UpdateLighting(Chunk& chunk);

private:
	void PropogateSunlight(Chunk& chunk);
	sf::Color InterpolateColor(float t) const;
};
