#pragma once
#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "engine/world/ChunksManager.h"
#include "engine/world/PerlinNoise.h"
#include "SoundManager.h"
#include "Item.h"

#include "EntityFactory.h"

#include "Systems/AISystem.h"
#include "Systems/CombatSystem.h"
#include "Systems/HealthSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/InventorySystem.h"

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

	//Setttings
	bool isLighting = true;
	bool showAIDebugLines = false;
	float masterVolume = 100.0f;

	Vec2 playerSpawnPos = { 0.0f, 0.0f };

	ChunksManager chunksManager;
	SoundManager soundManager;

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
	InventorySystem inventorySystem;

	//Inventory
	bool renderInventory = false;

	std::unordered_map<std::string, ItemDef> itemRegistry =
	{
		{"ShortSword", {1, false}},
		{"Torch", {64, true}},
		{"Grass", {64, true}},
		{"Dirt", {64, true}},
		{"Stone", {64, true}},
		{"Sand", {64, true}},
		{"SandStone", {64, true}},
		{"Snow", {64, true}},
		{"Ice", {64, true}},
	};	
	
	std::unordered_map<std::string, TileDef> tileRegistry =
	{
		{"Torch", {Tile::TileType::Torch, false}},
		{"Grass", {Tile::TileType::Grass, true}},
		{"Dirt", {Tile::TileType::Dirt, true}},
		{"Stone", {Tile::TileType::Stone, true}},
		{"Sand", {Tile::TileType::Sand, true}},
		{"SandStone", {Tile::TileType::SandStone, true}},
		{"Snow", {Tile::TileType::Snow, true}},
		{"Ice", {Tile::TileType::Ice, true}},
	};

	bool isMining = false;
	bool isPlacing = false;

	//Methods
	void Init();
	void ProcessEvents();
	void Update();
	void Render();

	void HandleMouseInput(const sf::Event event, ImGuiIO& io);
	void RenderHotbar();
	void RenderInventory();
	void RenderSettings();
};