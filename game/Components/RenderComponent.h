#include "SFML/Graphics.hpp"
struct RenderComponent
{
	sf::Sprite sprite;
	sf::Texture tex;
	bool facingRight = true;
};