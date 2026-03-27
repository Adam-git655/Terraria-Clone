#include "SFML/Graphics.hpp"
struct InputComponent
{
	bool movement_keys[sf::Keyboard::KeyCount] = { false };
};