#pragma once
#include "SFML/Graphics.hpp"

struct RenderComponent
{
	bool initialized = false;
	sf::Sprite sprite;
	sf::Vector2f scale;
	sf::Texture tex;
	bool facingRight = true;

	//flash effect
	bool isFlashing = false;
	float flashTimer = 0.0f;
	float flashDuration = 0.2f;
	sf::Color flashColor = sf::Color::White;
};