#include "zombie.h"

Zombie::Zombie(Vec2 p)
	:Entity(p)
{
	if (!tex.loadFromFile(RESOURCES_PATH "zombie.png"))
	{
		std::cout << "ERROR LOADING ZOMBIE TEX\n";
	}

	sprite.setTexture(tex);
	sprite.setScale(0.2f, 0.2f);
	sprite.setOrigin(sprite.getLocalBounds().getSize().x / 2, sprite.getLocalBounds().getSize().y / 2);
	sprite.setPosition(position.x, position.y);

	speed = 180.0f;
	max_speed = 280.0f;
	jumpStrength = 5.5f;

	visionRange = 600.0f;
	attackRange = 100.0f;
	damageAmount = 5.0f;
	attackCooldown = 2.0f;
}

void Zombie::update(float dt, Player& player, ChunksManager& chunksManager)
{
	Vec2 playerPos = player.getPosition();

	if (canSeePlayer(playerPos))
	{
		if (position.x == prevPos.x && IsOnGround)
		{
			velocity.y = -jumpStrength;
			IsOnGround = false;
		}
	}

	prevPos = position;

	if (!IsOnGround)
		velocity.y += gravity * dt;

	velocity.x = 0.0f;

	if (canSeePlayer(playerPos))
	{
		if (playerPos.x > position.x)
		{
			velocity.x += speed * dt;
			sprite.setScale(-0.2f, 0.2f);
		}
		if (playerPos.x < position.x)
		{
			velocity.x -= speed * dt;
			sprite.setScale(0.2f, 0.2f);
		}

		//cap speed
		if (velocity.x > max_speed)
			velocity.x = max_speed;
		if (velocity.x < -max_speed)
			velocity.x = -max_speed;
	}

	if (velocity.y > max_speed)
		velocity.y = max_speed;

	if (canAttackPlayer(playerPos))
	{
		if (attackClock.getElapsedTime().asSeconds() >= attackCooldown)
		{
			attackPlayer(player);
			attackClock.restart();
		}	
	}

	//add velocity to current position
	position += velocity;
	sprite.setPosition(position.x, position.y);
	chunksManager.collisionsWithTerrain(*this);

	position = Vec2(sprite.getPosition().x, sprite.getPosition().y);
}

void Zombie::attackPlayer(Player& player) const
{
	player.takeDamage(damageAmount);
}

bool Zombie::canSeePlayer(const Vec2& playerPos) const
{
	float squaredDistanceToPlayer = position.distSquared(playerPos);
	return squaredDistanceToPlayer <= (visionRange * visionRange);
}

bool Zombie::canAttackPlayer(const Vec2& playerPos) const
{
	float squaredDistanceToPlayer = position.distSquared(playerPos);
	return squaredDistanceToPlayer <= (attackRange * attackRange);
}