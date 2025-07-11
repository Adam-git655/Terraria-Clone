#include "player.h"
#include "ChunksManager.h"

Player::Player(Vec2 p)
	:Entity(p)
{
	if (!tex.loadFromFile(RESOURCES_PATH "player.png"))
	{
		std::cout << "ERROR LOADING PLAYER TEX\n";
	}
	if (!shortSwordTex.loadFromFile(RESOURCES_PATH "ShortSword.png"))
	{
		std::cout << "ERROR LOADING WEAPON TEX\n";
	}

	sprite.setTexture(tex);
	sprite.setOrigin(sprite.getLocalBounds().getSize().x / 2, sprite.getLocalBounds().getSize().y / 2);
	sprite.setPosition(position.x, position.y);

	speed = 200.0f;
	max_speed = 300.0f;
	jumpStrength = 6.0f;
}

Tile::TileType Player::getBlockTypeInHand() const
{
	return blockTypeInHand;
}

const sf::Texture& Player::getWeaponTexture(std::string& weaponName) const
{
	if (weaponName == "ShortSword")
		return shortSwordTex;
	std::cout << "ERROR\n";
}

void Player::setBlockTypeInHand(Tile::TileType type)
{
	blockTypeInHand = type;
}

void Player::set_movement_key(int code, bool b)
{
	movement_keys[code] = b;
}

void Player::takeDamage(float damage)
{
	health -= damage;
	std::cout << health << "\n";
}

void Player::update(float dt, ChunksManager& chunksManager)
{
	prevPos = position;

	if (health <= 0)
	{
		position = Vec2(0.0f, 0.0f);
		health = 100;
	}

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
		sprite.setScale(-1, 1);
	}
	if (movement_keys[sf::Keyboard::D])
	{
		velocity.x += speed * dt;
		sprite.setScale(1, 1);
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
	chunksManager.collisionsWithTerrain(*this); //check for collisions with tiles

	position = Vec2(sprite.getPosition().x, sprite.getPosition().y);
}