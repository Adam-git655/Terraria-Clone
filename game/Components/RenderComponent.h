#include "SFML/Graphics.hpp"
struct RenderComponent
{
	bool initialized = false;
	sf::Sprite sprite;
	sf::Vector2f scale;
	sf::Texture tex;
	bool facingRight = true;
};