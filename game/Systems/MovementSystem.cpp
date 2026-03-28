#include "MovementSystem.h"

void MovementSystem::update(EntityManager& mgr, float dt)
{
	auto& physicsStorage = mgr.getComponentStorage<PhysicsComponent>();
	auto& inputStorage = mgr.getComponentStorage<InputComponent>();
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();
	auto& movementStorage = mgr.getComponentStorage<MovementComponent>();
	auto& renderStorage = mgr.getComponentStorage<RenderComponent>();

	//Add Gravity to all entities which have a physics component
	for (auto& [e, physics] : physicsStorage.getAll())
	{
		if (!physics.IsOnGround)
			physics.velocity.y += physics.gravity * dt;
			
		if (physics.velocity.y > physics.terminalVelocity)
			physics.velocity.y = physics.terminalVelocity;

		if (physics.velocity.y > 0.5f)
			physics.IsFalling = true;
		else
			physics.IsFalling = false;
	}

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
	}
}