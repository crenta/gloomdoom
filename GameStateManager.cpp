// GameStateManager.cpp

#include "GameStateManager.h"


GameStateManager::GameStateManager() : currentState(GameState::DISPLAY)
{
	//need additions
}

void GameStateManager::changeState(GameState newState) 
{
    currentState = newState;

}

void GameStateManager::refresh() 
{
    switch (currentState) 
	{
		case GameState::DISPLAY:
			//need additions
            break;
			
        case GameState::GAME_IN_PROGRESS:
			//need additions
            break;
			
        case GameState::NEXT_LEVEL:
			//need additions
			break;
			
        case GameState::GAME_OVER:
			//need additions
            break;
    }
}

void GameStateManager::graphicRendering() 
{
	if(currentState == GameState::GAME_IN_PROGRESS)
	{
	//need additions
    }

}

void GameStateManager::startNewLevel(int newLevel) 
{
    level = newLevel;
    enemies.clear();
    towers.clear();
	currency = 100;
    time = 0.0f;
	currentState = GameState::GAME_IN_PROGRESS;
}

void GameStateManager::advanceToNextLevel()
{
	level++;
	if( level <= 3 ) 
	{
		currentState = GameState::NEXT_LEVEL;
	}
	else
	{
		currentState = GameState::GAME_OVER;
	}
}