## Overview
A simple Terraria clone made using C++ and the SFML graphics library

<img width="1908" height="967" alt="image" src="https://github.com/user-attachments/assets/b68d63c2-904e-405a-9cb3-37bdb80ca014" />
<img width="1914" height="986" alt="Screenshot 2025-11-01 234522" src="https://github.com/user-attachments/assets/c90130ef-771c-4013-9d3c-7a58d943de78" />
<img width="1912" height="969" alt="Screenshot 2025-11-01 234647" src="https://github.com/user-attachments/assets/96442dc2-4d00-440a-a550-7233117bb935" />
<img width="1905" height="967" alt="Screenshot 2025-11-01 234738" src="https://github.com/user-attachments/assets/b9aa0363-a9f1-4994-a86e-5c946db8a147" />


## Steps To Build
1. Install CMake
2. Run the following commands in PowerShell/terminal in the folder where CMakeLists.txt is located
```
mkdir build
cd build
cmake ../
```

## Features
### Current Features

- Sprite rendering and player movement
- Player and zombie animations
- Infinite world generation using a chunk system
- Terrain generation using 1D perlin noise
- Cave generation using 2D perlin noise and drunkard's walk
- Tree generation with randomized shapes and sizes
- Player collisions with the tiled environment
- Smart Zombie AI using A* pathfinding
- Basic Combat System
- Lighting System using BFS Blood-fill algorithm.
- Ability to place and destroy blocks
- Can choose between 3 block types (Grass, dirt, and stone)
- Inventory/Hotbar system to keep track of items
  
### Features to add

- Multithreading
- Different block types and minerals
- Multiple biomes
- Audio system
  
## Controls

- W to Jump, A and D to move left and right
- Right-click to place blocks, left-click to destroy blocks/attack with sword
- Number keys to choose items in the hotbar


