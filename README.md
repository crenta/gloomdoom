# My Awesome Tower Defense Game

This is a tower defense game built using C++ and the SFML library. Defend your base against waves of incoming enemies by strategically placing towers.

## Description

In this game, players must prevent enemies (represented by balloons) from reaching their base. They can build towers on the map to automatically attack the approaching enemies. The game features different types of static environmental objects, animated elements, and basic sound effects.

## Features

* Dynamic enemy spawning via a wave manager.
* Towers that automatically target and attack enemies within range.
* Projectile system for tower attacks.
* Enemies follow a predefined path (inferred from `mPathManager`).
* Enemies can attack towers if they are within melee range or become aggroed.
* Static environmental decorations (rocks, trees, flowers).
* Simple animations for background elements (tumbleweeds, birds).
* Resource loading for textures and sounds.
* Basic mouse-based tower placement.
* Game loop with time management and event processing.

## Technologies Used

This game was developed using C++ and leverages the following libraries:

* **SFML (Simple and Fast Multimedia Library):**

The primary external library used for graphics, window management, event handling, audio, and potentially other multimedia tasks. 

* **The code explicitly uses SFML for:**

    * `SFML/Graphics.hpp`: Handling sprites, textures, windows, shapes, and rendering.
    * SFML Audio (inferred from `mBackgroundMusic`, `mTowerShotBuf`, etc., though the specific header isn't in the provided snippet, it's necessary for the audio functionality shown).

* **Standard C++ Libraries:**

    * `<iostream>`: Used for input and output operations, primarily for printing debug information to the console (`std::cout`, `std::cerr`).
    * `<stdexcept>`: Provides standard exception classes, used here for runtime error handling during resource loading (`std::runtime_error`).
    * `<vector>`: Used extensively for dynamic arrays to manage game objects like enemies, towers, projectiles, and static objects.
    * `<random>`: Used for generating random numbers, specifically for placing static environmental objects at random positions.
    * `<iomanip>`: Used for controlling the formatting of output, seen in the game loop's time printing (`std::fixed`, `std::setprecision`).
    * `<cmath>`: (Inferred from `std::sqrt`) Used for mathematical functions, likely for distance calculations (e.g., in enemy targeting).
    * `<algorithm>`: (Inferred from `std::remove_if`) Used for algorithms like removing elements from containers based on a condition.
    * `<limits>`: (Inferred from `std::numeric_limits`) Used to access properties of numeric types, like the maximum possible value for distance calculations.


**Prerequisites:**

* A C++ compiler (e.g., g++, Clang, MSVC)
* SFML library (version X.Y.Z - specify the version you used if possible)
* Make or another build tool


## File Structure

* `Game.hpp`: Contains the declaration of the `Game` class, which manages the main game loop, objects, and resources.
* `Game.cpp`: Contains the implementation of the `Game` class methods.
* (Other files like `Enemy.hpp`, `Tower.hpp`, `Projectile.hpp`, `WaveManager.hpp`, `PathManager.hpp`, `PlayerBase.hpp` are likely part of the project based on the code)
* `sprites/`: Directory containing game assets (textures, sounds).

## Credits

Crenta & the GloomDoom group from LSU