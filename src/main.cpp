#include "SFML/Graphics.hpp"
#include "player.h"
#include "ChunksManager.h"
#include "PerlinNoise.h"

int main()
{
    const unsigned int windowWidth = 1920;
    const unsigned int windowHeight = 1080;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "BAD TERRARIA CLONE");    
    window.setFramerateLimit(60);
    Player player(Vec2(200.0f, 400.0f));


    sf::View camera;
    camera.setSize(1280.0f, 720.0f);
    camera.setCenter(player.getSprite().getPosition());
    
    
    ChunksManager chunksManager(std::time(nullptr));

    sf::Clock clock;
    double lastTime = clock.getElapsedTime().asSeconds();
    double deltaTime;

    bool isMining = false;
    bool isPlacing = false;

    while (window.isOpen())
    {
        double currentTime = clock.getElapsedTime().asSeconds();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.setCenter(player.getSprite().getPosition());

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed)
            {
                player.set_movement_key(event.key.code, true);

                if (event.key.code == sf::Keyboard::Num1)
                    player.setBlockTypeInHand(Tile::TileType::Grass);
                if (event.key.code == sf::Keyboard::Num2)
                    player.setBlockTypeInHand(Tile::TileType::Dirt);
                if (event.key.code == sf::Keyboard::Num3)
                    player.setBlockTypeInHand(Tile::TileType::Stone);
            }

            if (event.type == sf::Event::KeyReleased)
            {
                player.set_movement_key(event.key.code, false);
            }

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


        player.update(static_cast<float>(deltaTime), chunksManager);

        window.clear(sf::Color(0, 191, 255));
        window.setView(camera);
        chunksManager.UpdateAndRenderChunks(player, window);
        window.draw(player.getSprite());
        window.display();
    }
    
    return 0;
}