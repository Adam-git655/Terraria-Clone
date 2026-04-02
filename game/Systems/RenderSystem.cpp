#include "RenderSystem.h"

void RenderSystem::init(EntityManager& mgr, Entt e, RenderComponent& render)
{
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();
	auto& animationStorage = mgr.getComponentStorage<AnimationComponent>();

	render.sprite.setTexture(render.tex);
	render.sprite.setScale(render.scale);
	render.sprite.setOrigin(render.sprite.getLocalBounds().getSize().x / 2, render.sprite.getLocalBounds().getSize().y / 2);

	if (transformStorage.has(e))
	{
		auto& transform = transformStorage.get(e);

		render.sprite.setPosition(transform.position.x, transform.position.y);
	}

	if (animationStorage.has(e))
	{
		auto& animation = animationStorage.get(e);

		render.sprite.setTextureRect(animation.rectSourceSprite);
	}

	render.initialized = true;
}

void RenderSystem::update(EntityManager& mgr)
{
	auto& renderStorage = mgr.getComponentStorage<RenderComponent>();
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();
	auto& animationStorage = mgr.getComponentStorage<AnimationComponent>();
	auto& physicsStorage = mgr.getComponentStorage<PhysicsComponent>();
	auto& movementStorage = mgr.getComponentStorage<MovementComponent>();

	//Updates sprites of entities with render component
	for (auto& [e, render] : renderStorage.getAll())
	{
		//intialize sprites of entities
		if (!render.initialized)
		{
			init(mgr, e, render);
		}

		//Update sprites look direction
		//To check: original facing direction of sprite resource
		if (render.facingRight)
			render.sprite.setScale(std::abs(render.scale.x), render.scale.y);
		else
			render.sprite.setScale(-std::abs(render.scale.x), render.scale.y);


		if (!transformStorage.has(e))
			continue;
		auto& transform = transformStorage.get(e);

		//Update position
		render.sprite.setPosition(transform.position.x, transform.position.y);


		if (!animationStorage.has(e))
			continue;
		auto& animation = animationStorage.get(e);

		//Update animation for entities with physics and movement
		if (!physicsStorage.has(e) || !movementStorage.has(e))
			continue;

		auto& physics = physicsStorage.get(e);
		auto& movement = movementStorage.get(e);

		if (!physics.IsFalling && !movement.IsJumping)
		{
			if (physics.velocity.x != 0)
			{
				if (animation.rectSourceSprite.left < animation.walkRectLeftFirst)
					animation.rectSourceSprite.left = animation.walkRectLeftFirst;

				if (animation.animClock.getElapsedTime().asSeconds() > animation.animationTime)
				{
					if (animation.rectSourceSprite.left >= animation.walkRectLeftLast)
						animation.rectSourceSprite.left = animation.walkRectLeftFirst;
					else
						animation.rectSourceSprite.left += animation.walkRectLeftMove;
					animation.animClock.restart();
				}
			}
			else
			{
				animation.rectSourceSprite = animation.defaultSpriteStand;
			}
		}
		else
		{
			animation.rectSourceSprite = animation.spriteJump;
		}

		render.sprite.setTextureRect(animation.rectSourceSprite);
	}
}

void RenderSystem::draw(EntityManager& mgr, sf::RenderWindow& window)
{
	auto& renderStorage = mgr.getComponentStorage<RenderComponent>();

	for (auto& [e, render] : renderStorage.getAll())
	{
		window.draw(render.sprite);
	}
}