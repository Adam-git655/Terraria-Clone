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
		render.sprite.setOrigin(animation.rectSourceSprite.width / 2.f, animation.rectSourceSprite.height / 2.f);
	}

	render.initialized = true;
}

void RenderSystem::update(EntityManager& mgr, float dt)
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
			render.sprite.setScale(-std::abs(render.scale.x), render.scale.y);
		else
			render.sprite.setScale(std::abs(render.scale.x), render.scale.y);


		if (!transformStorage.has(e))
			continue;
		auto& transform = transformStorage.get(e);

		//Update position
		render.sprite.setPosition(transform.position.x, transform.position.y);

		//Handle flashing
		if (render.isFlashing)
		{
			render.sprite.setColor(render.flashColor);
			render.flashTimer += dt;

			if (render.flashTimer >= render.flashDuration)
			{
				render.isFlashing = false;
				render.sprite.setColor(sf::Color::White);
			}
		}

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
			if (std::abs(physics.velocity.x) > 0.1f)
			{
				animation.play("walk");
			}
			else
			{
				animation.play("idle");
			}
		}
		else
		{
			animation.play("jump");
		}

		advanceAnimation(animation);
		render.sprite.setTextureRect(animation.rectSourceSprite);
		render.sprite.setOrigin(animation.rectSourceSprite.width / 2.f, animation.rectSourceSprite.height / 2.f);
	}
}

void RenderSystem::advanceAnimation(AnimationComponent& animation)
{
	auto it = animation.animations.find(animation.currentAnim);
	if (it == animation.animations.end())
		return;

	Animation& anim = it->second;

	//wait before advancing to next frame of animation
	if (animation.animClock.getElapsedTime().asSeconds() < anim.animationTime)
		return;
	animation.animClock.restart();

	//advance animation
	if (!anim.pingPongAnim)
	{
		animation.rectSourceSprite.left += anim.rectLeftMove;
		if (animation.rectSourceSprite.left >= anim.rectLeftLast)
		{
			if (animation.locked)
			{
				//One shot animation finished
				//Unlock animation system, and 'return' to avoid playing that anim again.

				animation.locked = false;
				animation.rectSourceSprite.left = anim.rectLeftFirst;
				return;
			}
			animation.rectSourceSprite.left = anim.rectLeftFirst;
		}
	}
	else
	{
		if (animation.goingRight)
		{
			animation.rectSourceSprite.left += anim.rectLeftMove;
			if (animation.rectSourceSprite.left >= anim.rectLeftLast)
				animation.goingRight = false;
		}
		else
		{
			animation.rectSourceSprite.left -= anim.rectLeftMove;
			if (animation.rectSourceSprite.left <= anim.rectLeftFirst)
				animation.goingRight = true;
		}
	}
}

void RenderSystem::draw(EntityManager& mgr, sf::RenderWindow& window)
{
	auto& renderStorage = mgr.getComponentStorage<RenderComponent>();

	for (auto& [e, render] : renderStorage.getAll())
	{
		window.draw(render.sprite);
	}

	if (showAIDebugLines)
	{
		auto& aiStorage = mgr.getComponentStorage<AIComponent>();
		for (auto& [e, ai] : aiStorage.getAll())
		{
			window.draw(ai.lines);
		}
	}
}