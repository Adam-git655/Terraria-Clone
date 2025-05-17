#pragma once
#include "Vec2.h"
#include "SFML/Graphics.hpp"
#include "iostream"
#include "tile.h"

class ChunksManager;

class Player
{
private:
	sf::Texture tex;
	sf::Sprite sprite;

	Tile::TileType blockTypeInHand = Tile::TileType::Stone;
	
	bool movement_keys[sf::Keyboard::KeyCount] = { false };

	const float speed = 200.0f;
	const float max_speed = 300.0f;
	const float jumpStrength = 6.0f;
	Vec2 position = { 0.0f, 0.0f };
	Vec2 prevPos = { 0.0f, 0.0f };
	Vec2 velocity = { 0.0f, 0.0f };
	bool IsOnGround = false;

	const float gravity = 9.8f;

public:
	Player(Vec2 p = Vec2(400.0f, 400.0f));

	const sf::Sprite& getSprite();

	Vec2 getVelocity() const;

	Vec2 getPrevPos() const;

	void setVelocity(Vec2 v);

	void setPosition(Vec2 p);

	void move(const Vec2& offset);

	bool getIsOnGround() const;

	void setIsOnGround(bool b);

	Tile::TileType getBlockTypeInHand() const;

	void setBlockTypeInHand(Tile::TileType type);

	void set_movement_key(int code, bool b);

	void update(float dt, ChunksManager& chunksManager);
};