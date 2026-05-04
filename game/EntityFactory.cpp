#include "EntityFactory.h"

Entt EntityFactory::createPlayer(Vec2& spawnPos, const sf::Texture& playerTex)
{
	Entt e = mgr.create();
	mgr.addComponent<TransformComponent>(e, { spawnPos, spawnPos });
	mgr.addComponent<PhysicsComponent>(e, {});
	mgr.addComponent<MovementComponent>(e, { 300.0f, 700.0f, 600.0f, false });
	mgr.addComponent<InputComponent>(e, {});
	mgr.addComponent<CollisionComponent>(e, { {0.0f, 0.0f, 40.0f, 52.0f } });
	mgr.addComponent<HealthComponent>(e, { 100.0f, 100.0f });
	mgr.addComponent<InventoryComponent>(e, {});
	mgr.addComponent<RenderComponent>(e, { false, {}, {2.0f, 2.0f}, playerTex, true });
	mgr.addComponent<FactionComponent>(e, { Faction::Player });

	AnimationComponent anim;
	anim.addAnimation("idle", {0, 0, 0, 10, 20, 26, 0.035f, false});
	anim.addAnimation("walk", { 252, 492, 20, 10, 20, 26, 0.035f, false });
	anim.addAnimation("jump", { 232, 232, 0, 10, 20, 26, 0.035f, false });
	anim.addAnimation("swing", {20, 179, 53, 0, 53, 36, 0.045f, false});
	anim.play("idle");
	mgr.addComponent<AnimationComponent>(e, anim);

	return e;
}

Entt EntityFactory::createZombie(Vec2& spawnPos, const sf::Texture& zombieTex)
{
	Entt e = mgr.create();
	mgr.addComponent<TransformComponent>(e, { spawnPos, spawnPos });
	mgr.addComponent<PhysicsComponent>(e, {});
	mgr.addComponent<MovementComponent>(e, { 200.0f, 600.0f, 600.0f, false });
	mgr.addComponent<AIComponent>(e, {});
	mgr.addComponent<CollisionComponent>(e, { {0.0f, 0.0f, 39.25f, 53.25f} });
	mgr.addComponent<HealthComponent>(e, { 30.0f, 30.0f });
	mgr.addComponent<RenderComponent>(e, { false, {}, {0.25f, 0.25f}, zombieTex, true });
	mgr.addComponent<FactionComponent>(e, { Faction::Enemy });
	mgr.addComponent<WeaponComponent>(e, { "fist", 5.0f, {}, 1.75f, 50.0f, false });

	AnimationComponent anim;
	anim.addAnimation("idle", { 0, 0, 0, 0, 157, 213, 0.06f, false });
	anim.addAnimation("walk", {0, 314, 157, 0, 157, 213, 0.06f, true});
	anim.play("idle");
	mgr.addComponent<AnimationComponent>(e, anim);

	return e;
}

Entt EntityFactory::createBloodBat(Vec2& spawnPos, const sf::Texture& bloodBatTex)
{
	Entt e = mgr.create();
	mgr.addComponent<TransformComponent>(e, { spawnPos, spawnPos });
	mgr.addComponent<PhysicsComponent>(e, { false });
	mgr.addComponent<MovementComponent>(e, { 200.0f, 350.0f });
	mgr.addComponent<AirAIComponent>(e, {});
	mgr.addComponent<HealthComponent>(e, { 20.0f, 20.0f });
	mgr.addComponent<RenderComponent>(e, { false, {}, {1.15f, 1.15f}, bloodBatTex, true });
	mgr.addComponent<FactionComponent>(e, { Faction::Enemy });
	mgr.addComponent<WeaponComponent>(e, { "teeth", 3.0f, {}, 1.5f, 100.0f, false });

	AnimationComponent anim;
	anim.addAnimation("flight", { 0, 1050, 150, 0, 150, 150, 0.06f, false });
	anim.addAnimation("attack", { 0, 1050, 150, 150, 150, 150, 0.06f, false });
	anim.addAnimation("hit", { 0, 450, 150, 300, 150, 150, 0.06f, false });
	anim.addAnimation("death", { 0, 450, 150, 450, 150, 150, 0.06f, false });
	anim.play("flight");
	mgr.addComponent<AnimationComponent>(e, anim);

	return e;
}
