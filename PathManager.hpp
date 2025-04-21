#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>

class Enemy; 

class PathManager {
public:
    std::vector<sf::Vector2f> waypoints;

    PathManager(); 

    // get the starting position
    sf::Vector2f getStartPoint() const;
    bool updatePosition(Enemy& enemy, float deltaTime); 
};