#include "zombie.h"

Zombie::Zombie(Vec2 p)
	:Entity(p), ai(*this)
{
	if (!tex.loadFromFile(RESOURCES_PATH "zombieSheet.png"))
	{
		std::cout << "ERROR LOADING ZOMBIE TEX\n";
	}

	sprite.setTexture(tex);
	sprite.setTextureRect(rectSourceSprite);
	sprite.setScale(0.25f, 0.25f);
	sprite.setOrigin(sprite.getLocalBounds().getSize().x / 2, sprite.getLocalBounds().getSize().y / 2);
	sprite.setPosition(position.x, position.y);
	
	health = 30.0f;
	speed = 180.0f;
	max_speed = 280.0f;
	jumpStrength = 9.2f;

	visionRange = 1200.0f;
	attackRange = 50.0f;
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

	//DUM POSITION FOLLOW
	//if (!IsOnGround)
	//	velocity.y += gravity * dt;

	//velocity.x = 0.0f;

	//if (canSeePlayer(playerPos))
	//{
	//	if (playerPos.x >= position.x)
	//	{
	//		velocity.x += speed * dt;
	//		sprite.setScale(-0.25f, 0.25f);
	//	}
	//	else
	//	{
	//		velocity.x -= speed * dt;
	//		sprite.setScale(0.25f, 0.25f);
	//	}

	//	if (prevPos == position)
	//	{
	//		velocity.y = -jumpStrength;
	//	}

	//	//cap speed
	//	if (velocity.x > max_speed)
	//		velocity.x = max_speed;
	//	if (velocity.x < -max_speed)
	//		velocity.x = -max_speed;
	//}

	//if (velocity.y > max_speed)
	//	velocity.y = max_speed;

	prevPos = position;

	//SMORT A* PATH FOLLOW
	if (!IsOnGround)
		velocity.y += gravity * dt;

	velocity.x = 0.0f;

	if (canSeePlayer(playerPos))
	{		
		ai.update(playerPos, chunksManager, window, dt);

		//cap speed
		if (velocity.x > max_speed)
			velocity.x = max_speed;
		if (velocity.x < -max_speed)
			velocity.x = -max_speed;
	}

	if (velocity.y > max_speed)
		velocity.y = max_speed;

	if (velocity.y > 0.5f)
	{
		IsFalling = true;
	}
	else
	{
		IsFalling = false;
	}

	if (velocity.x != 0)
	{
		if (zombieWalkClock.getElapsedTime().asSeconds() >= 0.06f)
		{
			if (walkSpriteSheetGoRight)
			{
				rectSourceSprite.left += 157;
				if (rectSourceSprite.left >= 314)
					walkSpriteSheetGoRight = false;				
			}
			else
			{
				rectSourceSprite.left -= 157;
				if (rectSourceSprite.left <= 0)
					walkSpriteSheetGoRight = true;
			}
			zombieWalkClock.restart();
		}
	}
	else
	{
		rectSourceSprite = { 0, 0, 157, 213 };
	}

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
	sprite.setTextureRect(rectSourceSprite);
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

	if (delta.x != 0 && !IsJumping)
	{
		if (abs(delta.x) <= 1)
			velocity.x += speed * delta.x * dt;
		else
		{
			if (!IsJumping && !IsFalling)
			{
				IsJumping = true;
				velocity.y = -jumpStrength;
				IsOnGround = false;
			}
		}

		if (delta.x >= 0)
			sprite.setScale(-0.25f, 0.25f);
		else
			sprite.setScale(0.25f, 0.25f);
	}

	if (delta.y > 0 && !IsJumping)
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
			IsJumping = true;
			velocity.y = -jumpStrength;
			IsOnGround = false;
		}
	}

	if (IsJumping)
	{
		if (sprite.getScale().x < 0)
			velocity.x += 100 * dt;
		else
			velocity.x -= 100 * dt;

		if (IsOnGround)
			IsJumping = false;
	}
}

