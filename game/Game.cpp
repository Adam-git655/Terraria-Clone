#include "Game.h"

Game::Game(unsigned int width, unsigned int height)
	:windowWidth(width), windowHeight(height),
	chunksManager(std::time(nullptr))
{
	window.create(sf::VideoMode(windowWidth, windowHeight), "BAD TERRARIA CLONE");
	Init();
}	

Game::~Game()
{
	ImGui::SFML::Shutdown();
}

void Game::Init()
{
	//Initialize settings
	window.setFramerateLimit(0);
	camera.setSize(1920.0f, 1080.0f);
	ImGui::SFML::Init(window);

	//intialize textures
	if (!playerTex.loadFromFile(RESOURCES_PATH "playerSheet2.png"))
	{
		std::cout << "ERROR LOADING PLAYER TEX\n";
	}
	if (!zombieTex.loadFromFile(RESOURCES_PATH "zombieSheet.png"))
	{
		std::cout << "ERROR LOADING ZOMBIE TEX\n";
	}
	if (!shortSwordTex.loadFromFile(RESOURCES_PATH "ShortSword.png"))
	{
		std::cout << "ERROR LOADING SHORT SWORD TEX\n";
	}

	//Create player
	playerEntity = entityFactory.createPlayer(playerSpawnPos, playerTex);

	//Initialize hotbar
	hotbar.push_back(std::make_unique<WeaponItem>("ShortSword"));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Grass, true));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Dirt, true));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Stone, true));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Torch, false));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Sand, true));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::SandStone, true));

	lastTime = gameClock.getElapsedTime().asSeconds();
}

void Game::Run()
{
	while (window.isOpen())
	{
		double currentTime = gameClock.getElapsedTime().asSeconds();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		ProcessEvents();
		Update();
		Render();
	}
}

void Game::ProcessEvents()
{
	sf::Event event;
	ImGuiIO& io = ImGui::GetIO();

	while (window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed)
			window.close();

		auto& input = entityManager.getComponentStorage<InputComponent>().get(playerEntity);

		if (event.type == sf::Event::KeyPressed)
		{
			input.movement_keys[event.key.code] = true;

			if (event.key.code == sf::Keyboard::Num1)
				selectedIndex = 0;
			if (event.key.code == sf::Keyboard::Num2)
				selectedIndex = 1;
			if (event.key.code == sf::Keyboard::Num3)
				selectedIndex = 2;
			if (event.key.code == sf::Keyboard::Num4)
				selectedIndex = 3;
			if (event.key.code == sf::Keyboard::Num5)
				selectedIndex = 4;
			if (event.key.code == sf::Keyboard::Num6)
				selectedIndex = 5;
			if (event.key.code == sf::Keyboard::Num7)
				selectedIndex = 6;
		}

		if (event.type == sf::Event::KeyReleased)
			input.movement_keys[event.key.code] = false;

		HandleMouseInput(event, io);
	}
}

void Game::HandleMouseInput(const sf::Event event, ImGuiIO& io)
{
	if (io.WantCaptureMouse)
		return;

	Item* currentItem = hotbar[selectedIndex].get();
	auto* tileItem = dynamic_cast<TileItem*>(currentItem);

	if (event.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2f pointWorldCoords = window.mapPixelToCoords
		({ event.mouseButton.x, event.mouseButton.y }, camera);

		if (event.mouseButton.button == sf::Mouse::Left)
		{
			if (currentItem->getItemType() == Item::ItemType::Weapon)
			{
				auto& weaponsStorage = entityManager.getComponentStorage<WeaponComponent>();

				if (weaponsStorage.has(playerEntity))
					weaponsStorage.get(playerEntity).attackRequested = true;
			}
			else
			{
				isMining = true;
				chunksManager.DestroyTile(pointWorldCoords);
			}
		}

		else if (event.mouseButton.button == sf::Mouse::Right
			&& currentItem->getItemType() == Item::ItemType::Tile)
		{
			isPlacing = true;
			chunksManager.PlaceTile(pointWorldCoords, blockTypeInHand, tileItem->getSolid());
		}
	}

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
			isMining = false;
		else if (event.mouseButton.button == sf::Mouse::Right)
			isPlacing = false;
	}

	//Drag mining/placing
	if (event.type == sf::Event::MouseMoved)
	{
		sf::Vector2f pointWorldCoords = window.mapPixelToCoords
		({ event.mouseMove.x, event.mouseMove.y }, camera);

		if (isMining)
			chunksManager.DestroyTile(pointWorldCoords);
		if (isPlacing)
			chunksManager.PlaceTile(pointWorldCoords, blockTypeInHand, tileItem->getSolid());
	}
}

void Game::Update()
{
	ImGui::SFML::Update(window, deltaClock.restart());

	//Update systems

	aiSystem.update(entityManager, chunksManager, playerEntity, deltaTime);
	movementSystem.update(entityManager, deltaTime);
	chunksManager.collisionsWithTerrain(entityManager.getComponentStorage<CollisionComponent>(), 
										entityManager.getComponentStorage<TransformComponent>(),
										entityManager.getComponentStorage<PhysicsComponent>());

	for (auto& pos : chunksManager.getZombieSpawnPositions())
		entityFactory.createZombie(pos, zombieTex);
	chunksManager.getZombieSpawnPositions().clear();

	combatSystem.update(entityManager);
	healthSystem.update(entityManager, playerSpawnPos);
	renderSystem.update(entityManager, deltaTime);

	auto& playerTransform = entityManager.getComponentStorage<TransformComponent>().get(playerEntity);
	camera.setCenter(sf::Vector2f(playerTransform.position.x, playerTransform.position.y));
}

void Game::RenderHotbar()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 window_pos = ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);

	ImGui::Begin("Hotbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
	for (int i = 0; i < hotbar.size(); i++)
	{
		if (i == selectedIndex)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
		else
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

		Item* item = hotbar[i].get();
		if (!item)
			continue;

		const sf::Texture* tex = nullptr;

		if (item->getItemType() == Item::ItemType::Tile)
			tex = &chunksManager.getTexture(item->getItemName());
		else if (item->getItemType() == Item::ItemType::Weapon)
			tex = &shortSwordTex;

		if (tex)
		{
			std::string buttonId = "##btn" + std::to_string(i);
			if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0.3f, 0.6f, 1.0f, 1.0f)))
			{
				selectedIndex = i;
			}
		}

		ImGui::PopStyleColor();

		if (i < hotbar.size() - 1)
			ImGui::SameLine();

	}

	ImGui::End();

	//EQUIP / UNEQUIP TILES/WEAPONS LOGIC
	Item* selectedItem = hotbar[selectedIndex].get();

	auto& weaponsStorage = entityManager.getComponentStorage<WeaponComponent>();

	if (selectedItem->getItemType() == Item::ItemType::Tile)
	{
		auto* tileItem = dynamic_cast<TileItem*>(selectedItem);
		if (tileItem)
			blockTypeInHand = tileItem->getTileType();

		if (weaponsStorage.has(playerEntity))
			entityManager.removeComponent<WeaponComponent>(playerEntity);
	}
	else if (selectedItem->getItemType() == Item::ItemType::Weapon)
	{
		if (selectedItem->getItemName() == "ShortSword" && !weaponsStorage.has(playerEntity))
			entityManager.addComponent<WeaponComponent>(playerEntity, { "ShortSword", 10.0f, {}, 0.7f, 105.0f, false });
	}
}

void Game::RenderSettings()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 window_pos = ImVec2(
		viewport->WorkPos.x + viewport->WorkSize.x - 10.0f,  // 10 px from right
		viewport->WorkPos.y + 10.0f                          // 10 px from top
	);
	ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);  // Pivot: right-top corner

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::Checkbox("Lighting", &isLighting))
	{
		if (isLighting)
			chunksManager.EnableLighting();
		else
			chunksManager.DisableLighting();
	}

	if (ImGui::Checkbox("Show Pathfinding lines", &showAIDebugLines))
	{
		renderSystem.showAIDebugLines = showAIDebugLines;
	}

	ImGui::End();
}

void Game::Render()
{
	window.clear(sf::Color(0, 191, 255)); //Sky color
	window.setView(camera);

	auto& transform = entityManager.getComponentStorage<TransformComponent>().get(playerEntity);

	chunksManager.UpdateAndRenderChunks(static_cast<float>(deltaTime), transform.position, window);
	renderSystem.draw(entityManager, window);

	RenderHotbar();
	RenderSettings();

	ImGui::SFML::Render(window);
	window.display();
}