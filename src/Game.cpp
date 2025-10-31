#include "Game.h"

Game::Game(unsigned int width, unsigned int height)
	:windowWidth(width), windowHeight(height),
	player(Vec2(0.0f, 0.0f)),
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
	window.setFramerateLimit(60);
	camera.setSize(1920.0f, 1080.0f);
	camera.setCenter(player.getSprite().getPosition());
	ImGui::SFML::Init(window);

	//Initialize hotbar
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Grass, true));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Dirt, true));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Stone, true));
	hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Torch, false));
	hotbar.push_back(std::make_unique<WeaponItem>("ShortSword"));

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

		if (event.type == sf::Event::KeyPressed)
		{
			player.set_movement_key(event.key.code, true);

			if (event.key.code == sf::Keyboard::Num1)
				selectedIndex = 0;
			if (event.key.code == sf::Keyboard::Num2)
				selectedIndex = 1;
			if (event.key.code == sf::Keyboard::Num3)
				selectedIndex = 2;
			if (event.key.code == sf::Keyboard::Num4)
				selectedIndex = 3;
		}

		if (event.type == sf::Event::KeyReleased)
			player.set_movement_key(event.key.code, false);

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
				player.handleWeaponAttack(pointWorldCoords, chunksManager);
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
			chunksManager.PlaceTile(pointWorldCoords, player.getBlockTypeInHand(), tileItem->getSolid());
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
			chunksManager.PlaceTile(pointWorldCoords, player.getBlockTypeInHand(), tileItem->getSolid());
	}
}

void Game::Update()
{
	ImGui::SFML::Update(window, deltaClock.restart());
	camera.setCenter(player.getSprite().getPosition());
	player.update(static_cast<float>(deltaTime), chunksManager);
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
			tex = &player.getWeaponTexture(item->getItemName());

		if (tex)
		{
			std::string buttonId = "##btn" + std::to_string(i);
			if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0.3f, 0.6f, 1.0f, 1.0f)))
			{
				selectedIndex = i;
			}
		}

		Item* selectedItem = hotbar[selectedIndex].get();
		if (selectedItem->getItemType() == Item::ItemType::Tile)
		{
			auto* tileItem = dynamic_cast<TileItem*>(selectedItem);
			if (tileItem)
				player.setBlockTypeInHand(tileItem->getTileType());

			player.unequipWeapon();
		}
		else if (selectedItem->getItemType() == Item::ItemType::Weapon)
		{
			if (selectedItem->getItemName() == "ShortSword" && !player.hasWeaponEquipped())
				player.equipWeapon(std::make_unique<ShortSword>(10.0f, 0.5f));
		}

		ImGui::PopStyleColor();

		if (i < hotbar.size() - 1)
			ImGui::SameLine();

	}

	ImGui::End();
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

	ImGui::End();
}

void Game::Render()
{
	window.clear(sf::Color(0, 191, 255)); //Sky color
	window.setView(camera);

	chunksManager.UpdateAndRenderChunks(static_cast<float>(deltaTime), player, window);
	window.draw(player.getSprite());

	RenderHotbar();
	RenderSettings();

	ImGui::SFML::Render(window);
	window.display();
}