#include "engine/core/Vec2.h"
#include "engine/world/Pathfinding.h"
#include "engine/world/ChunksManager.h"
#include "Components/AIComponent.h"

class AISystem
{
public:
	void update(EntityManager& mgr, ChunksManager& chunksManager, Entt playerE, float dt);

private:

	bool isAtTileCenter(const Vec2& position, const IVec2& tilePos, const float tolerance = 1.0f);
};