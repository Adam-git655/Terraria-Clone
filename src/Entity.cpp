#include "Entity.h"

Entity::Entity(Vec2 pos)
	:position(pos)
{

}

const sf::Sprite& Entity::getSprite() const
{
	return sprite;
}

Vec2 Entity::getPrevPos() const
{
	return prevPos;
}

Vec2 Entity::getVelocity() const
{
	return velocity;
}

void Entity::setVelocity(Vec2 v)
{
	velocity = v;
}

Vec2 Entity::getPosition() const
{
	return Vec2(sprite.getPosition().x, sprite.getPosition().y);
}

void Entity::setPosition(Vec2 p)
{
	sprite.setPosition(p.x, p.y);
}

bool Entity::getIsOnGround() const
{
	return IsOnGround;
}

void Entity::setIsOnGround(bool b)
{
	IsOnGround = b;
}