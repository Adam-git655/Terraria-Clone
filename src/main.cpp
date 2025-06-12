#include "SFML/Graphics.hpp"
#include "player.h"
#include "zombie.h"
#include "ChunksManager.h"
#include "PerlinNoise.h"

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

    std::array<Tile::TileType, 3> hotbar = {Tile::TileType::Grass, Tile::TileType::Dirt, Tile::TileType::Stone};
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
            }

            if (event.type == sf::Event::KeyReleased)
            {
                player.set_movement_key(event.key.code, false);
            }

            if (!io.WantCaptureMouse)
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        isMining = true;
                        const sf::Vector2i point = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                        sf::Vector2f pointWorldCoords = window.mapPixelToCoords(point, camera);
                        chunksManager.DestroyTile(pointWorldCoords);
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right)
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

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Hotbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
        for (int i = 0; i < 3; i++)
        {
            if (i == selectedIndex)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            else
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

            sf::Texture* tex = nullptr;

            const char* label = "";
            switch (hotbar[i])
            {
            case Tile::TileType::Grass:
                label = "Grass";
                tex = &chunksManager.getTexture("Grass");
                break;
            case Tile::TileType::Dirt:
                label = "Dirt";
                tex = &chunksManager.getTexture("Dirt");
                break;
            case Tile::TileType::Stone:
                label = "Stone";
                tex = &chunksManager.getTexture("Stone");
                break;
            default:
                break;
            }

            if (tex)
            {
                std::string buttonId = "##btn" + std::to_string(i);
                if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(50, 50)))
                {
                    selectedIndex = i;
                }
            }

            player.setBlockTypeInHand(hotbar[selectedIndex]);

            ImGui::PopStyleColor();

            if (i < 2)
                ImGui::SameLine();

        }
        ImGui::End();


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