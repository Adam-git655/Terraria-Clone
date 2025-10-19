#pragma once
#include "Vec2.h"
#include "SFML/Graphics.hpp"

class Entity
{
public:
	Entity(Vec2 pos = Vec2(0.0f, 0.0f));

	virtual ~Entity() = default;

	const sf::Sprite& getSprite() const;

	sf::FloatRect getSpriteBounds() const;

	Vec2 getVelocity() const;
	void setVelocity(Vec2 v);

	Vec2 getPosition() const;
	void setPosition(Vec2 p);

	Vec2 getPrevPos() const;

	bool getIsOnGround() const;
	void setIsOnGround(bool b);

protected:
	sf::Sprite sprite;

	float health;
	float speed = 200.0f;
	float max_speed = 300.0f;
	float jumpStrength = 6.0f;

	Vec2 position = { 0.0f, 0.0f };
	Vec2 prevPos = { 0.0f, 0.0f };
	Vec2 velocity = { 0.0f, 0.0f };
	bool IsOnGround = false;
	bool IsFalling = false;
	bool IsJumping = false;

	const float gravity = 13.5f;
};