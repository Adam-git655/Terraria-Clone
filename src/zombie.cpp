#include "zombie.h"

Zombie::Zombie(Vec2 p)
	:Entity(p)
{
	if (!tex.loadFromFile(RESOURCES_PATH "zombie.png"))
	{
		std::cout << "ERROR LOADING ZOMBIE TEX\n";
	}

	sprite.setTexture(tex);
	sprite.setScale(0.2f, 0.2f);
	sprite.setOrigin(sprite.getLocalBounds().getSize().x / 2, sprite.getLocalBounds().getSize().y / 2);
	sprite.setPosition(position.x, position.y);
	
	health = 30.0f;
	speed = 180.0f;
	max_speed = 280.0f;
	jumpStrength = 5.5f;

	visionRange = 600.0f;
	attackRange = 100.0f;
	damageAmount = 5.0f;
	attackCooldown = 2.0f;
}

void Zombie::takeDamage(float damage)
{
	health -= damage;
	//std::cout << "Zombie Health: " <<  health << "\n";
}

bool Zombie::isAlive() const
{
	return alive;
}

void Zombie::update(float dt, Player& player, ChunksManager& chunksManager)
{
	if (health <= 0)
	{
		alive = false;
	}

	Vec2 playerPos = player.getPosition();

	if (canSeePlayer(playerPos))
	{
		if (position.x == prevPos.x && IsOnGround)
		{
			velocity.y = -jumpStrength;
			IsOnGround = false;
		}
	}

	prevPos = position;

	if (!IsOnGround)
		velocity.y += gravity * dt;

	velocity.x = 0.0f;

	if (canSeePlayer(playerPos))
	{
		if (playerPos.x > position.x)
		{
			velocity.x += speed * dt;
			sprite.setScale(-0.2f, 0.2f);
		}
		if (playerPos.x < position.x)
		{
			velocity.x -= speed * dt;
			sprite.setScale(0.2f, 0.2f);
		}

		//cap speed
		if (velocity.x > max_speed)
			velocity.x = max_speed;
		if (velocity.x < -max_speed)
			velocity.x = -max_speed;
	}

	if (velocity.y > max_speed)
		velocity.y = max_speed;

	if (canAttackPlayer(playerPos))
	{
		if (attackClock.getElapsedTime().asSeconds() >= attackCooldown)
		{
			attackPlayer(player);
			attackClock.restart();
		}	
	}

	//add velocity to current position
	position += velocity;
	sprite.setPosition(position.x, position.y);
	chunksManager.collisionsWithTerrain(*this);

	position = Vec2(sprite.getPosition().x, sprite.getPosition().y);
}

void Zombie::attackPlayer(Player& player) const
{
	player.takeDamage(damageAmount);
}

bool Zombie::canSeePlayer(const Vec2& playerPos) const
{
	float squaredDistanceToPlayer = position.distSquared(playerPos);
	return squaredDistanceToPlayer <= (visionRange * visionRange);
}

bool Zombie::canAttackPlayer(const Vec2& playerPos) const
{
	float squaredDistanceToPlayer = position.distSquared(playerPos);
	return squaredDistanceToPlayer <= (attackRange * attackRange);
}

std::vector<Vec2> Zombie::getNeighbours(int x, int y, int maxJumpHeight, ChunksManager& chunksManager)
{
	std::vector<Vec2> neighboursPos;

	auto floorDiv = [](int a, int b) { return (a >= 0) ? a / b : ((a + 1) / b) - 1; };

	int left = x - 1;
	int right = x + 1;

	int currentChunkX = floorDiv(x, Chunk::CHUNK_WIDTH);
	int currentLocalX = x - currentChunkX * Chunk::CHUNK_WIDTH;
	auto& currentChunkTiles = chunksManager.getChunk(currentChunkX).getChunkTiles();
	bool isGrounded = currentChunkTiles[currentLocalX][y + 1].isSolid();

	int newChunkX = floorDiv(left, Chunk::CHUNK_WIDTH);
	int newLocalX = left - newChunkX * Chunk::CHUNK_WIDTH;

	if (!chunksManager.getChunk(newChunkX).getChunkTiles()[newLocalX][y].isSolid())
	{
		neighboursPos.push_back(Vec2(left, y));
	}

	newChunkX = floorDiv(right, Chunk::CHUNK_WIDTH);
	newLocalX = right - newChunkX * Chunk::CHUNK_WIDTH;

	if (!chunksManager.getChunk(newChunkX).getChunkTiles()[newLocalX][y].isSolid())
	{
		neighboursPos.push_back(Vec2(right, y));
	}

	if (isGrounded)
	{
		for (int jump = 1; jump <= maxJumpHeight; ++jump)
		{
			int up = y - jump;

			if (up < 0)
			{
				std::cout << "out of bounds neighbour check\n";
				break;
			}

			if (!currentChunkTiles[currentLocalX][up].isSolid())
				neighboursPos.push_back(Vec2(x, up));
			else 
				break;

		}
	}
	else
	{
		if (y + 1 < Chunk::CHUNK_HEIGHT)
		{
			neighboursPos.push_back(Vec2(x, y + 1));
		}
	}
	
	return neighboursPos;
}

std::vector<Vec2> Zombie::solveAStar(int startX, int startY, int goalX, int goalY, int maxJumpHeight, ChunksManager& chunksManager)
{
	std::priority_queue<Node*, std::vector<Node*>, CompareNode> openList; //make a queue which puts shorter global values at front
	std::unordered_map<Vec2, std::unique_ptr<Node>> nodes;
	std::unordered_set<Vec2> alreadyVisited;

	Vec2 startPos = Vec2(startX, startY);
	Vec2 goalPos = Vec2(goalX, goalY);

	nodes[startPos] = std::make_unique<Node>();
	Node* startNode = nodes[startPos].get();
	startNode->x = startX;
	startNode->y = startY;
	startNode->globalGoal = heuristic(Vec2(startNode->x, startNode->y), Vec2(goalX, goalY));
	startNode->localGoal = 0;
	startNode->parent = Vec2(-1, -1);

	openList.push(startNode);

	while (!openList.empty())
	{
		Node* current = openList.top();
		openList.pop();
		Vec2 currentPos = Vec2(current->x, current->y);

		if (alreadyVisited.find(currentPos) != alreadyVisited.end())
			continue;

		if (currentPos == goalPos)
		{
			std::vector<Vec2> path;
			while (currentPos != startPos)
			{
				path.push_back(currentPos);
				current = nodes[current->parent].get();
			}
			std::reverse(path.begin(), path.end());
			return path;
		}

		alreadyVisited.insert(Vec2(current->x, current->y));


		for (Vec2 neighbourPos : getNeighbours(current->x, current->y, maxJumpHeight, chunksManager))
		{
			if (alreadyVisited.find(neighbourPos) != alreadyVisited.end())
				continue;

			if (neighbourPos != startPos && nodes.find(neighbourPos) == nodes.end())
			{
				nodes[neighbourPos] = std::make_unique<Node>();
				Node* nodeNeighbour = nodes[neighbourPos].get();
				nodeNeighbour->localGoal = INFINITY;
				nodeNeighbour->globalGoal = INFINITY;
				nodeNeighbour->x = neighbourPos.x;
				nodeNeighbour->y = neighbourPos.y;
			}

			Node* nodeNeighbour = nodes[neighbourPos].get();
			int possiblyLowerGoal = current->localGoal + distance(currentPos, neighbourPos);

			if (possiblyLowerGoal < nodeNeighbour->localGoal)
			{
				nodeNeighbour->localGoal = possiblyLowerGoal;

				nodeNeighbour->globalGoal = nodeNeighbour->localGoal + heuristic(neighbourPos, goalPos);
				
				nodeNeighbour->parent = currentPos;

				openList.push(nodeNeighbour);
			}

		}
	}

	return {};
}