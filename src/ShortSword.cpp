#include "ShortSword.h"	

ShortSword::ShortSword(float damage, float attackCooldown)
	:Weapon(damage, attackCooldown)
{

}

void ShortSword::attack(Vec2 targetPos)
{
	if (attackClock.getElapsedTime().asSeconds() >= attackCooldown)
	{
		std::cout << "Attacked\n";
		attackClock.restart();
	}
}