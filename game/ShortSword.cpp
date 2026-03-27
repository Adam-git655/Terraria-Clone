#include "ShortSword.h"	

ShortSword::ShortSword(float damage, float attackCooldown)
	:Weapon(damage, attackCooldown)
{

}

void ShortSword::attack(sf::Vector2f targetPos, ChunksManager& chunksManager)
{
	if (attackClock.getElapsedTime().asSeconds() >= attackCooldown)
	{
		for (const auto& zombie : chunksManager.getAllZombies())
		{
			if (zombie)
			{
				if (zombie->getSpriteBounds().contains(targetPos))
				{
					zombie->takeDamage(damage);
				}
			}
		}

		attackClock.restart();
	}
}