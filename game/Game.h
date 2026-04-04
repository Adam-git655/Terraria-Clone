#pragma once
#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "engine/world/ChunksManager.h"
#include "engine/world/PerlinNoise.h"
#include "Item.h"
#include "TileItem.h"
#include "WeaponItem.h"

#include "EntityFactory.h"

#include "Systems/AISystem.h"
#include "Systems/CombatSystem.h"
#include "Systems/HealthSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"

#include <vector>

class Game
{
public:
	Game(unsigned int width, unsigned int height);
	~Game();

	void Run();

private:
	//Settings and stuff
	unsigned int windowWidth;
	unsigned int windowHeight;

	sf::RenderWindow window;
	sf::View camera;
	sf::Clock gameClock;

	double lastTime = 0.0;
	double deltaTime = 0.0;
	sf::Clock deltaClock;

	bool isLighting = true;
	bool showAIDebugLines = false;

	Vec2 playerSpawnPos = { 0.0f, 0.0f };

	ChunksManager chunksManager;

	//Textures
	sf::Texture playerTex;
	sf::Texture zombieTex;
	sf::Texture shortSwordTex;

	//ECS
	EntityManager entityManager;
	EntityFactory entityFactory{ entityManager };
	Entt playerEntity;

	//Systems
	AISystem aiSystem;
	CombatSystem combatSystem;
	HealthSystem healthSystem;
	MovementSystem movementSystem;
	RenderSystem renderSystem;

	//Hotbar
	std::vector<std::unique_ptr<Item>> hotbar;
	Tile::TileType blockTypeInHand = Tile::TileType::Grass;
	int selectedIndex = 0;
	bool isMining = false;
	bool isPlacing = false;

	//Methods
	void Init();
	void ProcessEvents();
	void Update();
	void Render();

	void HandleMouseInput(const sf::Event event, ImGuiIO& io);
	void RenderHotbar();
	void RenderSettings();
};