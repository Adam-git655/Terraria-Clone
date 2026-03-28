#include "engine/ecs/EntityManager.h"
#include "game/Components/TransformComponent.h"
#include "game/Components/MovementComponent.h"
#include "game/Components/PhysicsComponent.h"
#include "game/Components/InputComponent.h"
#include "game/Components/RenderComponent.h"

class MovementSystem
{
public:
	void update(EntityManager& mgr, float dt);
};
