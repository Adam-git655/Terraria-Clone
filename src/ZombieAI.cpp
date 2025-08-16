#include "ZombieAI.h"
#include "zombie.h"

ZombieAI::ZombieAI(Zombie& z)
	:controlledZombie(z)
{

}

void ZombieAI::update(Vec2& playerPos, ChunksManager& chunksManager, sf::RenderWindow& window,  float dt)
{
	int zombieGlobalTileX = static_cast<int>(std::floor(controlledZombie.getPosition().x / Chunk::TILESIZE));
	int zombieGlobalTileY = static_cast<int>(std::floor(controlledZombie.getPosition().y / Chunk::TILESIZE));
	int playerGlobalTileX = static_cast<int>(std::floor(playerPos.x / Chunk::TILESIZE));
	int playerGlobalTileY = static_cast<int>(std::floor(playerPos.y / Chunk::TILESIZE));

	path = solveAStar(zombieGlobalTileX, zombieGlobalTileY, playerGlobalTileX, playerGlobalTileY, 3, chunksManager);
	currentPathIndex = 1;

	//sf::VertexArray lines(sf::LineStrip, path.size());
	
	//for (size_t i = 0; i < path.size(); ++i)
	//{
	//	lines[i].position = sf::Vector2f(path[i].x * Chunk::TILESIZE + Chunk::TILESIZE / 2.0f, path[i].y * Chunk::TILESIZE + Chunk::TILESIZE / 2.0f);
	//	lines[i].color = sf::Color::Yellow;
	//}
	
	//window.draw(lines);


	if (!path.empty() && currentPathIndex < path.size())
	{
		IVec2 currentTile = path[currentPathIndex - 1];
		IVec2 nextTile = path[currentPathIndex];

		if (isAtTileCenter(controlledZombie.getPosition(), nextTile))
		{
			currentPathIndex++;
			if (currentPathIndex >= path.size())
			{
				controlledZombie.setVelocity(Vec2(0.0f, controlledZombie.getVelocity().y));
				return;
			}
			nextTile = path[currentPathIndex];
		}

		controlledZombie.followPath(currentTile, nextTile, dt);
	}
}

std::vector<IVec2> ZombieAI::solveAStar(int startX, int startY, int goalX, int goalY, int maxJumpHeight, ChunksManager& chunksManager)
{
	std::priority_queue<Node*, std::vector<Node*>, CompareNode> openList; //make a queue which puts shorter global values at front
	std::unordered_map<IVec2, std::unique_ptr<Node>> nodes;
	std::unordered_set<IVec2> alreadyVisited;

	int maxNodesToSearch = 250;
	int nodesSearched = 0;

	IVec2 startPos = IVec2(startX, startY);
	IVec2 goalPos = IVec2(goalX, goalY);

	auto startNode = std::make_unique<Node>();
	startNode->x = startX;
	startNode->y = startY;
	startNode->globalGoal = heuristic(startNode->x, startNode->y, goalX, goalY);
	startNode->localGoal = 0;
	startNode->parent = IVec2(-1, -1);

	nodes[startPos] = std::move(startNode);
	Node* startNodePtr = nodes[startPos].get();

	openList.push(startNodePtr);

	while (!openList.empty())
	{
		Node* current = openList.top();
		openList.pop();

		if (++nodesSearched > maxNodesToSearch)
			return {};

		IVec2 currentPos = IVec2(current->x, current->y);

		if (alreadyVisited.find(currentPos) != alreadyVisited.end())
			continue;

		if (currentPos == goalPos)
		{
			std::vector<IVec2> path;

			while (currentPos != startPos)
			{
				path.push_back(currentPos);

				auto it = nodes.find(current->parent);
				if (it == nodes.end())
				{
					std::cout << "ERR\n";
					break;
				}
				current = it->second.get();
				currentPos = IVec2(current->x, current->y);
			}

			path.push_back(startPos);
			std::reverse(path.begin(), path.end());
			return path;
		}


		assert(current != nullptr);
		alreadyVisited.insert(IVec2(current->x, current->y));


		for (IVec2 neighbourPos : getNeighbours(current->x, current->y, maxJumpHeight, chunksManager))
		{
			if (alreadyVisited.find(neighbourPos) != alreadyVisited.end())
				continue;

			if (neighbourPos != startPos && nodes.find(neighbourPos) == nodes.end())
			{
				auto nodeNeighbour = std::make_unique<Node>();
				nodeNeighbour->localGoal = INFINITY;
				nodeNeighbour->globalGoal = INFINITY;
				nodeNeighbour->x = neighbourPos.x;
				nodeNeighbour->y = neighbourPos.y;

				nodes[neighbourPos] = std::move(nodeNeighbour);
			}

			Node* nodeNeighbour = nodes[neighbourPos].get();
			int possiblyLowerGoal = current->localGoal + distance(currentPos.x, currentPos.y, neighbourPos.x, neighbourPos.y);

			if (possiblyLowerGoal < nodeNeighbour->localGoal)
			{
				nodeNeighbour->localGoal = possiblyLowerGoal;

				nodeNeighbour->globalGoal = nodeNeighbour->localGoal + heuristic(neighbourPos.x, neighbourPos.y, goalPos.x, goalPos.y);

				nodeNeighbour->parent = currentPos;

				assert(nodes.find(currentPos) != nodes.end() && "Parent node was not in the node map!");
				if (nodes.find(currentPos) == nodes.end())
				{
					auto copy = std::make_unique<Node>();
					copy->x = current->x;
					copy->y = current->y;
					copy->globalGoal = current->globalGoal;
					copy->localGoal = current->localGoal;
					copy->parent = current->parent;
					nodes[currentPos] = std::move(copy);
				}

				openList.push(nodeNeighbour);
			}

		}
	}

	return {};
}


std::vector<IVec2> ZombieAI::getNeighbours(int x, int y, int maxJumpHeight, ChunksManager& chunksManager)
{
	std::vector<IVec2> neighboursPos;

	int left = x - 1;
	int right = x + 1;

	if (isTileBelowGround(x, y, chunksManager))
	{
		//LEFT RIGHT NEIGHBOURS

		if (isFree(left, y, chunksManager))
		{
			neighboursPos.push_back(IVec2(left, y));
		}
		if (isFree(right, y, chunksManager))
		{
			neighboursPos.push_back(IVec2(right, y));
		}

		//JUMP NEIGHBOURS 

		for (int i = 1; i <= maxJumpHeight; ++i)
		{
			int up = y - i;

			if (up < 0)
				break;

			if (isFree(x, up, chunksManager))
			{
				neighboursPos.push_back(IVec2(x, up));
			}
			else
			{
				break;
			}
		}

	}
	else
	{
		//JUMP'S LEFT AND RIGHT NEIGHBOURS
		for (int dir = -1; dir <= 1; dir += 2)
		{
			for (int j = 1; j <= 4; ++j)
			{
				int newX = x + (j * dir);

				if (isFree(newX, y, chunksManager))
				{
					if (isTileBelowGround(newX, y, chunksManager))
						neighboursPos.push_back(IVec2(newX, y));
				}
				else
				{
					break;
				}
			}
		}


		//DOWN NEIGHBOURS
		if (y + 1 < Chunk::CHUNK_HEIGHT)
		{
			neighboursPos.push_back(IVec2(x, y + 1));
		}
	}

	return neighboursPos;
}


bool ZombieAI::isAtTileCenter(const Vec2& position, const IVec2& tilePos, const float tolerance)
{
	float tileCenterX = static_cast<float>(tilePos.x * Chunk::TILESIZE + Chunk::TILESIZE / 2.0f);
	float tileCenterY = static_cast<float>(tilePos.y * Chunk::TILESIZE + Chunk::TILESIZE / 2.0f);

	float dx = std::abs(position.x - tileCenterX);
	float dy = std::abs(position.y - tileCenterY);

	return dx <= tolerance && dy <= tolerance;
}

bool ZombieAI::isFree(int x, int y, ChunksManager& chunksManager)
{
	auto floorDiv = [](int a, int b) { return (a >= 0) ? a / b : ((a + 1) / b) - 1; };

	if (y < 0 || y >= Chunk::CHUNK_HEIGHT)
		return false;

	int chunkX = floorDiv(x, Chunk::CHUNK_WIDTH);
	int localX = x - chunkX * Chunk::CHUNK_WIDTH;

	return !chunksManager.getChunk(chunkX).getChunkTiles()[localX][y].isSolid();
}

bool ZombieAI::isTileBelowGround(int x, int y, ChunksManager& chunksManager)
{
	int belowY = y + 1;
	return !isFree(x, belowY, chunksManager);
}