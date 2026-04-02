#include "engine/ecs/EntityManager.h"
#include "game/Components/RenderComponent.h"
#include "game/Components/TransformComponent.h"
#include "game/Components/AnimationComponent.h"
#include "game/Components/PhysicsComponent.h"
#include "game/Components/MovementComponent.h"

class RenderSystem
{
public:
	void update(EntityManager& mgr);
	void draw(EntityManager& mgr, sf::RenderWindow& window);

private:
	void init(EntityManager& mgr, Entt e, RenderComponent& render);
};