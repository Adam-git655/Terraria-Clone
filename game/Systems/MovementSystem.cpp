#include "MovementSystem.h"

void MovementSystem::update(EntityManager& mgr, float dt)
{
	auto& physicsStorage = mgr.getComponentStorage<PhysicsComponent>();
	auto& inputStorage = mgr.getComponentStorage<InputComponent>();
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();
	auto& movementStorage = mgr.getComponentStorage<MovementComponent>();
	auto& renderStorage = mgr.getComponentStorage<RenderComponent>();
	auto& aiStorage = mgr.getComponentStorage<AIComponent>();

	//Add Gravity to all entities which have a physics component
	for (auto& [e, physics] : physicsStorage.getAll())
	{
		auto& transform = transformStorage.get(e);

		if (!physics.IsOnGround)
			physics.velocity.y += physics.gravity * dt;
			
		if (physics.velocity.y > physics.terminalVelocity)
			physics.velocity.y = physics.terminalVelocity;

		if (std::abs(transform.position.y - transform.prevPos.y) > 1.0f)
			physics.IsFalling = true;
		else
			physics.IsFalling = false;
	}

	Vec2 playerPos = { 0, 0 };

	//Move entities with input component (player)
	for (auto& [e, input] : inputStorage.getAll())
	{
		auto& physics = physicsStorage.get(e);
		auto& transform = transformStorage.get(e);
		auto& movement = movementStorage.get(e);
		auto& render = renderStorage.get(e);

		transform.prevPos = transform.position;
		physics.velocity.x = 0.0f;

		if ((input.movement_keys[sf::Keyboard::W] || input.movement_keys[sf::Keyboard::Space]) && !movement.IsJumping && !physics.IsFalling)
		{
			physics.velocity.y = -movement.jumpStrength;
			physics.IsOnGround = false;
			movement.IsJumping = true;
		}
		if (input.movement_keys[sf::Keyboard::A])
		{
			physics.velocity.x -= movement.speed;
			render.facingRight = false;
		}
		if (input.movement_keys[sf::Keyboard::D])
		{
			physics.velocity.x += movement.speed;
			render.facingRight = true;
		}

		if (physics.IsOnGround)
			movement.IsJumping = false;

		if (physics.velocity.x > movement.max_speed)
			physics.velocity.x = movement.max_speed;
		if (physics.velocity.x < -movement.max_speed)
			physics.velocity.x = -movement.max_speed;

		transform.position += physics.velocity * dt;

		playerPos = transform.position;
	}

	//Move entities with AI component (zombies)
	for (auto& [e, ai] : aiStorage.getAll())
	{
		auto& physics = physicsStorage.get(e);
		auto& transform = transformStorage.get(e);
		auto& movement = movementStorage.get(e);
		auto& render = renderStorage.get(e);

		transform.prevPos = transform.position;
		physics.velocity.x = 0.0f;

		if (ai.canSeePlayer)
		{
			zombieFollowPath(physics, movement, render, ai.currentTile, ai.nextTile);

			if (physics.velocity.x > movement.max_speed)
				physics.velocity.x = movement.max_speed;
			if (physics.velocity.x < -movement.max_speed)
				physics.velocity.x = -movement.max_speed;
		}

		transform.position += physics.velocity * dt;
	}
}

void MovementSystem::zombieFollowPath(PhysicsComponent& physics, MovementComponent& movement, RenderComponent& render, const IVec2& currentTile, const IVec2& nextTile)
{
	IVec2 delta = nextTile - currentTile;

	if (delta.x != 0 && !movement.IsJumping)
	{
		if (abs(delta.x) <= 1)
			physics.velocity.x += movement.speed * delta.x;
		else
		{
			if (!movement.IsJumping && !physics.IsFalling)
			{
				movement.IsJumping = true;
				physics.velocity.y = -movement.jumpStrength;
				physics.IsOnGround = false;
			}
		}

		if (delta.x >= 0)
			render.facingRight = true;
		else
			render.facingRight = false;
	}

	if (delta.y > 0 && !movement.IsJumping)
	{
		if (render.facingRight)
			physics.velocity.x += movement.speed;
		else
			physics.velocity.x -= movement.speed;
	}

	if (delta.y < 0)
	{
		if (physics.IsOnGround)
		{
			movement.IsJumping = true;
			physics.velocity.y = -movement.jumpStrength;
			physics.IsOnGround = false;
		}
	}

	if (movement.IsJumping)
	{
		if (render.facingRight)
			physics.velocity.x += movement.speed;
		else
			physics.velocity.x -= movement.speed;

		if (physics.IsOnGround)
			movement.IsJumping = false;
	}
}

