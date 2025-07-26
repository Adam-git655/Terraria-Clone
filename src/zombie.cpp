#include "zombie.h"

Zombie::Zombie(Vec2 p)
	:Entity(p), ai(*this)
{
	if (!tex.loadFromFile(RESOURCES_PATH "zombie.png"))
	{
		std::cout << "ERROR LOADING ZOMBIE TEX\n";
	}

	sprite.setTexture(tex);
	sprite.setScale(0.2f, 0.2f);
	sprite.setOrigin(sprite.getLocalBounds().getSize().x / 2, sprite.getLocalBounds().getSize().y / 2);
	sprite.setPosition(position.x, position.y);
	
	health = 30.0f;
	speed = 180.0f;
	max_speed = 280.0f;
	jumpStrength = 6.1f;

	visionRange = 600.0f;
	attackRange = 100.0f;
	damageAmount = 5.0f;
	attackCooldown = 2.0f;
}

void Zombie::takeDamage(float damage)
{
	health -= damage;
	//std::cout << "Zombie Health: " <<  health << "\n";
}

bool Zombie::isAlive() const
{
	return alive;
}

void Zombie::update(float dt, Player& player, ChunksManager& chunksManager, sf::RenderWindow& window)
{
	if (health <= 0)
	{
		alive = false;
	}

	Vec2 playerPos = player.getPosition();

	prevPos = position;

	if (!IsOnGround)
		velocity.y += gravity * dt;

	velocity.x = 0.0f;

	if (canSeePlayer(playerPos))
	{		
		//SMORT A* PATH FOLLOW
		ai.update(playerPos, chunksManager, window, dt);

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

void Zombie::followPath(const IVec2& currentTile, const IVec2& nextTile, float dt)
{
	IVec2 delta = nextTile - currentTile;

	if (delta.x != 0 && !isJumping)
	{
		velocity.x += speed * delta.x * dt;

		if (delta.x >= 0)
			sprite.setScale(-0.2f, 0.2f);
		else
			sprite.setScale(0.2f, 0.2f);
	}

	if (delta.y > 0 && !isJumping)
	{
		if (sprite.getScale().x < 0)
			velocity.x += speed * dt;
		else
			velocity.x -= speed * dt;	
	}

	if (delta.y < 0)
	{
		if (IsOnGround)
		{
			isJumping = true;
			velocity.y = -jumpStrength;
			IsOnGround = false;
		}
	}

	if (isJumping)
	{
		if (sprite.getScale().x < 0)
			velocity.x += 100 * dt;
		else
			velocity.x -= 100 * dt;

		if (IsOnGround)
			isJumping = false;
	}
}

