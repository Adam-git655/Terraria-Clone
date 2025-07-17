#include "SFML/Graphics.hpp"
#include "player.h"
#include "zombie.h"
#include "ChunksManager.h"
#include "PerlinNoise.h"
#include "ShortSword.h"

#include "Item.h"
#include "TileItem.h"
#include "WeaponItem.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include <array>

int main()
{
    const unsigned int windowWidth = 1920;
    const unsigned int windowHeight = 1080;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "BAD TERRARIA CLONE");    
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);
    ImGuiIO& io = ImGui::GetIO();

    Player player(Vec2(0.0f, 0.0f));

    sf::View camera;
    camera.setSize(1280.0f, 720.0f);
    camera.setCenter(player.getSprite().getPosition());
        
    ChunksManager chunksManager(std::time(nullptr));
    
    sf::Clock clock;
    double lastTime = clock.getElapsedTime().asSeconds();
    double deltaTime;

    std::vector<std::unique_ptr<Item>> hotbar;
    hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Grass));
    hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Dirt));
    hotbar.push_back(std::make_unique<TileItem>(Tile::TileType::Stone));
    hotbar.push_back(std::make_unique<WeaponItem>("ShortSword"));
    int selectedIndex = 0;
    bool isMining = false;
    bool isPlacing = false;

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        double currentTime = clock.getElapsedTime().asSeconds();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.setCenter(player.getSprite().getPosition());

#pragma region InputHandling
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
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
            {
                player.set_movement_key(event.key.code, false);
            }

            if (!io.WantCaptureMouse)
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    Item* currentItem = hotbar[selectedIndex].get();
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        const sf::Vector2i point = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                        sf::Vector2f pointWorldCoords = window.mapPixelToCoords(point, camera);

                        if (currentItem->getItemType() == Item::ItemType::Weapon)
                        {
                            player.handleWeaponAttack(pointWorldCoords, chunksManager);
                        }
                        else
                        {
                            isMining = true;
                            chunksManager.DestroyTile(pointWorldCoords);
                        }
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right && currentItem->getItemType() == Item::ItemType::Tile)
                    {
                        isPlacing = true;
                        const sf::Vector2i point = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                        sf::Vector2f pointWorldCoords = window.mapPixelToCoords(point, camera);
                        chunksManager.PlaceTile(pointWorldCoords, player.getBlockTypeInHand());
                    }

                }

                if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        isMining = false;
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        isPlacing = false;
                    }
                }
            }

            if (isMining)
            {
                if (event.type == sf::Event::MouseMoved)
                {
                    const sf::Vector2i point = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
                    sf::Vector2f pointWorldCoords = window.mapPixelToCoords(point, camera);
                    chunksManager.DestroyTile(pointWorldCoords);
                }
            }

            if (isPlacing)
            {
                if (event.type == sf::Event::MouseMoved)
                {
                    const sf::Vector2i point = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
                    sf::Vector2f pointWorldCoords = window.mapPixelToCoords(point, camera);
                    chunksManager.PlaceTile(pointWorldCoords, player.getBlockTypeInHand());
                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

#pragma endregion

#pragma region HotbarSystem
        ImGui::SFML::Update(window, deltaClock.restart());

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
            {
                tex = &chunksManager.getTexture(item->getItemName());
            }

            else if (item->getItemType() == Item::ItemType::Weapon)
            {
                tex = &player.getWeaponTexture(item->getItemName());
            }

            if (tex)
            {
                std::string buttonId = "##btn" + std::to_string(i);
                if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(50, 50), ImVec2(0,0), ImVec2(1,1), -1, ImVec4(0.3f, 0.6f, 1.0f, 1.0f)))
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
                if (selectedItem->getItemName() == "ShortSword")
                {
                    if (!player.hasWeaponEquipped())
                        player.equipWeapon(std::make_unique<ShortSword>(10.0f, 0.5f));
                }
            }

            ImGui::PopStyleColor();

            if (i < hotbar.size() - 1)
                ImGui::SameLine();

        }
        ImGui::End();



#pragma endregion

        player.update(static_cast<float>(deltaTime), chunksManager);

        window.clear(sf::Color(0, 191, 255));
        window.setView(camera);
        chunksManager.UpdateAndRenderChunks(static_cast<float>(deltaTime), player, window);
        window.draw(player.getSprite());
        ImGui::SFML::Render(window);
        window.display();
    }
    
    ImGui::SFML::Shutdown();
    return 0;
}