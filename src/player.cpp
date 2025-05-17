#include "player.h"
#include "ChunksManager.h"

Player::Player(Vec2 p)
	:position(p)
{
	if (!tex.loadFromFile(RESOURCES_PATH "player.png"))
	{
		std::cout << "ERROR LOADING PLAYER TEX\n";
	}

	sprite.setTexture(tex);
	sprite.setOrigin(sprite.getLocalBounds().getSize().x / 2, sprite.getLocalBounds().getSize().y / 2);
	sprite.setPosition(position.x, position.y);
}

const sf::Sprite& Player::getSprite()
{
	return sprite;
}

Vec2 Player::getPrevPos() const
{
	return prevPos;
}

Vec2 Player::getVelocity() const
{
	return velocity;
}

void Player::setVelocity(Vec2 v)
{
	velocity = v;
}

void Player::setPosition(Vec2 p)
{
	sprite.setPosition(p.x, p.y);
}

void Player::move(const Vec2& offset)
{
	sprite.move(offset.x, offset.y);
}

bool Player::getIsOnGround() const
{
	return IsOnGround;
}

void Player::setIsOnGround(bool b)
{
	IsOnGround = b;
}

Tile::TileType Player::getBlockTypeInHand() const
{
	return blockTypeInHand;
}

void Player::setBlockTypeInHand(Tile::TileType type)
{
	blockTypeInHand = type;
}

void Player::set_movement_key(int code, bool b)
{
	movement_keys[code] = b;
}

void Player::update(float dt, ChunksManager& chunksManager)
{

	prevPos = position;

	//Add gravity
	if (!IsOnGround)
		velocity.y += gravity * dt;

	velocity.x = 0.0f;

	//Input
	if ((movement_keys[sf::Keyboard::W] || movement_keys[sf::Keyboard::Space]) && IsOnGround)
	{
		velocity.y = -jumpStrength;
		IsOnGround = false;
	}
	if (movement_keys[sf::Keyboard::A])
	{
		velocity.x -= speed * dt;
	}
	if (movement_keys[sf::Keyboard::D])
	{
		velocity.x += speed * dt;
	}

	//Cap velocity 
	if (velocity.x > max_speed)
		velocity.x = max_speed;
	if (velocity.x < -max_speed)
		velocity.x = -max_speed;
	if (velocity.y > max_speed)
		velocity.y = max_speed;

	//Add velocity to position
	position += velocity;
	sprite.setPosition(position.x, position.y);
	chunksManager.playerCollision(*this); //check for collisions with tiles

	position = Vec2(sprite.getPosition().x, sprite.getPosition().y);
	
}