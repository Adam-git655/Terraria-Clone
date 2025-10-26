#pragma once
#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "player.h"
#include "zombie.h"
#include "ChunksManager.h"
#include "PerlinNoise.h"
#include "ShortSword.h"
#include "Item.h"
#include "TileItem.h"
#include "WeaponItem.h"

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

	//Entities
	Player player;
	ChunksManager chunksManager;

	//Hotbar
	std::vector<std::unique_ptr<Item>> hotbar;
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
};