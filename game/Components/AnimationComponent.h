#include "SFML/Graphics.hpp"

struct AnimationComponent
{
	sf::IntRect defaultSpriteStand = { 0, 0, 20, 26 };
	sf::IntRect spriteJump = { 0, 0, 0, 0 };
	sf::IntRect rectSourceSprite = { 100, 0, 20, 26 };

	int walkRectLeftFirst = 120;
	int walkRectLeftLast = 360;
	int walkRectLeftMove = 20;

	sf::Clock animClock;
	float animationTime = 0.035f;

	bool pingPongAnim = false;
	bool walkSpriteSheetGoRight = true;
};