# CMakeLists.txt

# Minimum CMake version required
cmake_minimum_required(VERSION 3.10...3.25)


# Project Name and Language
project(GloomDoom LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Fetch SFML using FetchContent ---
include(FetchContent)

FetchContent_Declare(
  SFML           
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG        2.6.x
)

FetchContent_MakeAvailable(SFML)


# --- Game Executable ---
add_executable(${PROJECT_NAME}
    # CPP source files
    main.cpp
    Game.cpp
    Enemy.cpp
    Tower.cpp
    PlayerBase.cpp
    PathManager.cpp
    WaveManager.cpp
    Projectile.cpp
    # HPP source files
    Game.hpp
    Enemy.hpp
    Tower.hpp
    PlayerBase.hpp
    PathManager.hpp
    WaveManager.hpp
    Projectile.hpp
)

# --- Add SFML Include Directory Explicitly --- 
target_include_directories(${PROJECT_NAME} PUBLIC "${sfml_SOURCE_DIR}/include")

# --- Link SFML Libraries to the Game ---
target_link_libraries(${PROJECT_NAME} PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
)
