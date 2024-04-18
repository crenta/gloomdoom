// GameStateManager.h
#pragma once
#include <SFML/Graphics.hpp>
#include "GameEntities.h"

enum class GameState {DISPLAY, GAME_IN_PROGRESS, NEXT_LEVEL, GAME_OVER};

class GameStateManager {
public:
    GameStateManager();
    void handleEvent(const sf::Event& event);
    void refresh();
    void graphicRendering(sf::RenderWindow& window);
    void startNewLevel(int level);

private:
    GameState currentState;
    std::vector<Enemy> enemies;
    std::vector<Tower> towers;
    PlayerBase playerBase;
};

// GameStateManager.cpp
#include "GameStateManager.h"

GameStateManager::GameStateManager() : currentState(GameState::DISPLAY) {}

void GameStateManager::handleEvent(const sf::Event& event) {
    // Event handling based on the current game state
}

void GameStateManager::refresh() {
    // Update game logic based on the current state
}

void GameStateManager::graphicRendering(sf::RenderWindow& window) {
    // Render game elements based on the current state
}

void GameStateManager::startNewLevel(int level) {
    // Initialize a new level
}