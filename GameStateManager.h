// GameStateManager.h

#pragma once
#include <vector>

class Tower;
class Enemy;

enum class GameState {DISPLAY, GAME_IN_PROGRESS, NEXT_LEVEL, GAME_OVER };

class GameStateManager 
{
public:
    GameStateManager();
    void changeState(GameState gameState = GameState::DISPLAY);
    void refresh();
    void graphicRendering();
    void startNewLevel(int newLevel);
	void advanceToNextLevel();
	
private:
	GameState currentState;
	int level;
	int health;
	int currency;
	std::vector<Tower> towers;
	std::vector<Enemy> enemies;
	bool enemiesIncoming;
	float time;
};