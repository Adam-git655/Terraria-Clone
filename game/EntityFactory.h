#pragma once
#include "engine/ecs/EntityManager.h"

#include "game/Components/AIComponent.h"
#include "game/Components/AirAIComponent.h"
#include "game/Components/AnimationComponent.h"
#include "game/Components/CollisionComponent.h"
#include "game/Components/FactionComponent.h"
#include "game/Components/HealthComponent.h"
#include "game/Components/InputComponent.h"
#include "game/Components/InventoryComponent.h"
#include "game/Components/MovementComponent.h"
#include "game/Components/PhysicsComponent.h"
#include "game/Components/RenderComponent.h"
#include "game/Components/TransformComponent.h"
#include "game/Components/WeaponComponent.h"

class EntityFactory
{
public:
	EntityFactory(EntityManager& mgr)
		:mgr(mgr)
	{
	}

	Entt createPlayer(Vec2& spawnPos, const sf::Texture& playerTex);
	Entt createZombie(Vec2& spawnPos, const sf::Texture& zombieTex);
	Entt createBloodBat(Vec2& spawnPos, const sf::Texture& bloodBatTex);

private:
	EntityManager& mgr;
};