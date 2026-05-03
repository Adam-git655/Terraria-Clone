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
	if (!bloodBatTex.loadFromFile(RESOURCES_PATH "BloodBatSpriteSheet.png"))
	{
		std::cout << "ERROR LOADING BLOOD BAT TEX\n";
	}
	if (!shortSwordTex.loadFromFile(RESOURCES_PATH "ShortSword.png"))
	{
		std::cout << "ERROR LOADING SHORT SWORD TEX\n";
	}

	//Create player
	playerEntity = entityFactory.createPlayer(playerSpawnPos, playerTex);
	
	//Initialize hotbar
	inventorySystem.addItem(entityManager, playerEntity, "ShortSword", 1, itemRegistry);
	inventorySystem.addItem(entityManager, playerEntity, "Torch", 64, itemRegistry);

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
		auto& inv = entityManager.getComponentStorage<InventoryComponent>().get(playerEntity);

		if (event.type == sf::Event::KeyPressed)
		{
			input.movement_keys[event.key.code] = true;

			if (event.key.code == sf::Keyboard::Num1)
				inv.activeHotbarSlot = 0;
			if (event.key.code == sf::Keyboard::Num2)
				inv.activeHotbarSlot = 1;
			if (event.key.code == sf::Keyboard::Num3)
				inv.activeHotbarSlot = 2;
			if (event.key.code == sf::Keyboard::Num4)
				inv.activeHotbarSlot = 3;
			if (event.key.code == sf::Keyboard::Num5)
				inv.activeHotbarSlot = 4;
			if (event.key.code == sf::Keyboard::Num6)
				inv.activeHotbarSlot = 5;
			if (event.key.code == sf::Keyboard::Num7)
				inv.activeHotbarSlot = 6;
			if (event.key.code == sf::Keyboard::Num8)
				inv.activeHotbarSlot = 7;
			if (event.key.code == sf::Keyboard::Num9)
				inv.activeHotbarSlot = 8;
			if (event.key.code == sf::Keyboard::Num0)
				inv.activeHotbarSlot = 9;

			if (event.key.code == sf::Keyboard::E)
			{
				renderInventory = !renderInventory;
				inv.selectedInventorySlot = -1;
			}
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

	if (renderInventory)
		return;

	auto& inv = entityManager.getComponentStorage<InventoryComponent>().get(playerEntity);

	if (event.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2f pointWorldCoords = window.mapPixelToCoords
		({ event.mouseButton.x, event.mouseButton.y }, camera);

		if (event.mouseButton.button == sf::Mouse::Left)
		{
			ItemStack& item = inv.slots[inv.activeHotbarSlot];

			//Attack
			if (!itemRegistry[item.itemId].isBlock)
			{
				auto& weaponsStorage = entityManager.getComponentStorage<WeaponComponent>();
				
				if (weaponsStorage.has(playerEntity))
					weaponsStorage.get(playerEntity).attackRequested = true;
			}
			//Mine
			else
			{
				isMining = true;
				Tile::TileType tileRemoved = chunksManager.DestroyTile(pointWorldCoords);
				std::string itemId = Tile::tileTypeToItemId(tileRemoved);
				if (!itemId.empty())
				{
					soundManager.play(Sounds::Break);
					inventorySystem.addItem(entityManager, playerEntity, itemId, 1, itemRegistry);
				}
			}
		}

		//Place
		else if (event.mouseButton.button == sf::Mouse::Right)
		{
			ItemStack& item = inv.slots[inv.activeHotbarSlot];
			//Ensure item exists in current equipped slot
			if (itemRegistry.count(item.itemId) == 0)
				return;

			//Ensure item is block
			if (!itemRegistry[inv.slots[inv.activeHotbarSlot].itemId].isBlock)
				return;

			if (item.count > 0)
			{
				isPlacing = true;
				bool placed = chunksManager.PlaceTile(pointWorldCoords, tileRegistry[item.itemId].type, tileRegistry[item.itemId].isSolid);
				if (placed)
				{
					soundManager.play(Sounds::Place);
					inventorySystem.removeItem(entityManager, playerEntity, item.itemId, 1);
				}
			}
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
		{
			Tile::TileType tileRemoved = chunksManager.DestroyTile(pointWorldCoords);
			std::string itemId = Tile::tileTypeToItemId(tileRemoved);
			if (!itemId.empty())
			{
				soundManager.play(Sounds::Break);
				inventorySystem.addItem(entityManager, playerEntity, itemId, 1, itemRegistry);
			}
		}
		if (isPlacing)
		{
			ItemStack& item = inv.slots[inv.activeHotbarSlot];
			if (item.count > 0 && itemRegistry.count(item.itemId) && itemRegistry[item.itemId].isBlock)
			{
				bool placed = chunksManager.PlaceTile(pointWorldCoords, tileRegistry[item.itemId].type, tileRegistry[item.itemId].isSolid);
				if (placed)
				{
					soundManager.play(Sounds::Place);
					inventorySystem.removeItem(entityManager, playerEntity, item.itemId, 1);
				}
			}
		}
	}
}

void Game::Update()
{
	ImGui::SFML::Update(window, deltaClock.restart());

	//Update systems

	aiSystem.update(entityManager, chunksManager, playerEntity, deltaTime);
	movementSystem.update(entityManager, soundManager, deltaTime);
	chunksManager.collisionsWithTerrain(entityManager.getComponentStorage<CollisionComponent>(), 
										entityManager.getComponentStorage<TransformComponent>(),
										entityManager.getComponentStorage<PhysicsComponent>());

	for (auto& pos : chunksManager.getZombieSpawnPositions())
		entityFactory.createZombie(pos, zombieTex);
	chunksManager.getZombieSpawnPositions().clear();

	for (auto& pos : chunksManager.getBloodBatSpawnPositions())
		entityFactory.createBloodBat(pos, bloodBatTex);
	chunksManager.getBloodBatSpawnPositions().clear();

	combatSystem.update(entityManager, soundManager);
	healthSystem.update(entityManager, playerSpawnPos);
	renderSystem.update(entityManager, deltaTime);

	auto& playerTransform = entityManager.getComponentStorage<TransformComponent>().get(playerEntity);
	camera.setCenter(sf::Vector2f(playerTransform.position.x, playerTransform.position.y));
}

void Game::RenderHotbar()
{
	auto& inv = entityManager.getComponentStorage<InventoryComponent>().get(playerEntity);

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	//set position to top left
	ImVec2 window_pos = ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);

	const float SLOT_SIZE = 50.0f;

	ImGui::Begin("Hotbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
	for (int i = 0; i < inv.HOTBAR_SIZE; i++)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		ItemStack& item = inv.slots[i];

		//draw empty slots
		if (item.count == 0)
		{
			std::string buttonId = "##btn" + std::to_string(i);
			if (ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE))) //normal button
				inv.activeHotbarSlot = i;
		}
		else
		{
			//if not empty, get texture of item in slot
			const sf::Texture* tex = nullptr;

			if (itemRegistry[item.itemId].isBlock)
				tex = &chunksManager.getTexture(item.itemId);
			else
				tex = &shortSwordTex;

			std::string buttonId = "##btn" + std::to_string(i);
			if (tex)
			{
				//draw texture through image button
				ImGui::PushID(i);
				if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(SLOT_SIZE, SLOT_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
					inv.activeHotbarSlot = i;
				ImGui::PopID();
			}
			else
			{
				//fallback, wont happen likely
				if (ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE)))
					inv.activeHotbarSlot = i;
			}
		}

		ImGui::PopStyleVar();

		//add boundary rect when hovered
		ImVec2 rectMin = ImGui::GetItemRectMin();
		ImVec2 rectMax = ImGui::GetItemRectMax();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (i == inv.activeHotbarSlot)
			drawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);
		else if (ImGui::IsItemHovered())
			drawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

		//add slot number on top left
		std::string numStr = std::to_string(i + 1);
		if (numStr == "10")
			numStr = "0";

		ImVec2 textPos = ImVec2(rectMin.x + 3, rectMin.y + 3);
		//Shadow
		drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), numStr.c_str());
		//White text on top
		drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), numStr.c_str());

		//add item count on bottom right
		if (item.count > 1)
		{
			std::string countStr = std::to_string(item.count);
			ImVec2 textSize = ImGui::CalcTextSize(countStr.c_str());
			textPos = ImVec2(rectMax.x - textSize.x - 5, rectMax.y - textSize.y - 3);

			//Shadow
			drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), countStr.c_str());
			//White text on top
			drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), countStr.c_str());
		}

		//ensure same line
		if (i < inv.HOTBAR_SIZE - 1)
			ImGui::SameLine();
	}

	ImGui::End();

	//EQUIP / UNEQUIP WEAPONS LOGIC
	ItemStack& item = inv.slots[inv.activeHotbarSlot];
	auto& weaponsStorage = entityManager.getComponentStorage<WeaponComponent>();

	if (itemRegistry.count(item.itemId) == 0)
	{
		//empty slot, uneqip weapon
		if (weaponsStorage.has(playerEntity))
			entityManager.removeComponent<WeaponComponent>(playerEntity);

	}
	else if (itemRegistry[item.itemId].isBlock)
	{
		//uneqip weapon
		if (weaponsStorage.has(playerEntity))
			entityManager.removeComponent<WeaponComponent>(playerEntity);
	}
	else
	{
		//equips weapon because sword
		if (item.itemId == "ShortSword" && !weaponsStorage.has(playerEntity))
			entityManager.addComponent<WeaponComponent>(playerEntity, { "ShortSword", 10.0f, {}, 0.7f, 105.0f, false });
	}
}

void Game::RenderInventory()
{
	if (!renderInventory)
		return;

	auto& inv = entityManager.getComponentStorage<InventoryComponent>().get(playerEntity);

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	//set position to middle of screen
	ImVec2 window_pos = ImVec2(
		viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
		viewport->WorkPos.y + viewport->WorkSize.y * 0.5f
	);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

	const int COLS = 10;
	const float SLOT_SIZE = 50.0f;

	for (int i = 0; i < inv.INVENTORY_SIZE; ++i)
	{
		int slotIndex = inv.HOTBAR_SIZE + i;
		ItemStack& item = inv.slots[slotIndex];

		int col = i % COLS;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		if (item.count == 0)
		{
			std::string buttonId = "##btn" + std::to_string(slotIndex);

			if (ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE)))
			{
				if (inv.selectedInventorySlot != -1)
				{
					//move item from old slot to empty slot
					std::swap(inv.slots[inv.selectedInventorySlot], item);
					inv.selectedInventorySlot = -1;
				}
			}
		}
		else
		{
			const sf::Texture* tex = nullptr;

			if (itemRegistry[item.itemId].isBlock)
				tex = &chunksManager.getTexture(item.itemId);
			else
				tex = &shortSwordTex;

			std::string buttonId = "##btn" + std::to_string(slotIndex);
			if (tex)
			{
				ImGui::PushID(slotIndex);
				if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(SLOT_SIZE, SLOT_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
				{
					if (inv.selectedInventorySlot == -1)
					{
						//select slot first time
						if (item.count > 0)
							inv.selectedInventorySlot = slotIndex;
					}
					else
					{
						//swap item from old slot with new slot
						std::swap(inv.slots[inv.selectedInventorySlot], item);
						inv.selectedInventorySlot = -1;
					}
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE));
			}
		}

		ImGui::PopStyleVar();

		ImVec2 rectMin = ImGui::GetItemRectMin();
		ImVec2 rectMax = ImGui::GetItemRectMax();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (slotIndex == inv.selectedInventorySlot)
			drawList->AddRect(rectMin, rectMax, IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f);
		if (ImGui::IsItemHovered())
			drawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

		if (item.count > 1)
		{
			std::string countStr = std::to_string(item.count);
			ImVec2 textSize = ImGui::CalcTextSize(countStr.c_str());
			ImVec2 textPos = ImVec2(rectMax.x - textSize.x - 5, rectMax.y - textSize.y - 3);

			//Shadow
			drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), countStr.c_str());
			//White text on top
			drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), countStr.c_str());
		}

		if (col < COLS - 1)
			ImGui::SameLine();
	}

	ImGui::End();


	//Draw another hotbar above inventory with some gap

	ImVec2 hotbar_win_pos = ImVec2(
		viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
		(viewport->WorkPos.y + viewport->WorkSize.y * 0.5f) - (2 * 50.0f + 50.0f)
	);
	ImGui::SetNextWindowPos(hotbar_win_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Hotbar-Inv", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

	for (int i = 0; i < inv.HOTBAR_SIZE; i++)
	{
		ItemStack& item = inv.slots[i];

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		if (item.count == 0)
		{
			std::string buttonId = "##btn" + std::to_string(i);
			if (ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE))) //normal button
			{
				if (inv.selectedInventorySlot != -1)
				{
					std::swap(inv.slots[inv.selectedInventorySlot], item);
					inv.selectedInventorySlot = -1;
				}
			}
		}
		else
		{
			const sf::Texture* tex = nullptr;

			if (itemRegistry[item.itemId].isBlock)
				tex = &chunksManager.getTexture(item.itemId);
			else
				tex = &shortSwordTex;

			std::string buttonId = "##btn" + std::to_string(i);
			if (tex)
			{
				ImGui::PushID(i);
				if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(SLOT_SIZE, SLOT_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
				{
					if (inv.selectedInventorySlot == -1)
					{
						if (item.count > 0)
							inv.selectedInventorySlot = i;
					}
					else
					{
						std::swap(inv.slots[inv.selectedInventorySlot], item);
						inv.selectedInventorySlot = -1;
					}
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE));
			}
		}

		ImGui::PopStyleVar();

		ImVec2 rectMin = ImGui::GetItemRectMin();
		ImVec2 rectMax = ImGui::GetItemRectMax();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (i == inv.selectedInventorySlot)
			drawList->AddRect(rectMin, rectMax, IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f);
		if (ImGui::IsItemHovered())
			drawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

		//add slot number on top left
		std::string numStr = std::to_string(i + 1);
		if (numStr == "10")
			numStr = "0";

		ImVec2 textPos = ImVec2(rectMin.x + 3, rectMin.y + 3);
		//Shadow
		drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), numStr.c_str());
		//White text on top
		drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), numStr.c_str());

		//add item count on bottom right
		if (item.count > 1)
		{
			std::string countStr = std::to_string(item.count);
			ImVec2 textSize = ImGui::CalcTextSize(countStr.c_str());
			textPos = ImVec2(rectMax.x - textSize.x - 5, rectMax.y - textSize.y - 3);

			drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), countStr.c_str());
			drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), countStr.c_str());
		}

		//ensure same line
		if (i < inv.HOTBAR_SIZE - 1)
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

	if (ImGui::Checkbox("Show Pathfinding lines", &showAIDebugLines))
	{
		renderSystem.showAIDebugLines = showAIDebugLines;
	}

	if (ImGui::SliderFloat("Master Volume", &masterVolume, 0.0f, 100.0f))
	{
		soundManager.setMasterVolume(masterVolume);
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
	RenderInventory();
	RenderSettings();

	ImGui::SFML::Render(window);
	window.display();
}