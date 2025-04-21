#include "PathManager.hpp"
#include "Enemy.hpp"
#include <cmath>
#include <iostream>

PathManager::PathManager() {
    // hardcoded path for the map
    waypoints = {
        sf::Vector2f(0, 540),
        sf::Vector2f(250, 540),
        sf::Vector2f(250, 300),
        sf::Vector2f(1750, 300) 
    };
}

// get the starting point
sf::Vector2f PathManager::getStartPoint() const {
    if (waypoints.empty()) {
        // if no waypoints are defined... error
        std::cerr << "Error: PathManager has no waypoints!" << std::endl;
        return sf::Vector2f(0, 0); 
    }
    return waypoints.front();
}

// update the position
bool PathManager::updatePosition(Enemy& enemy, float deltaTime) {
    if (enemy.isDead) return true;

    // have they reached the end?
    if (enemy.waypointIndex >= waypoints.size() - 1) {
        enemy.startAttacking();
        return true;
    }

    // read the next waypoint
    sf::Vector2f currentTarget   = waypoints[enemy.waypointIndex + 1];
    sf::Vector2f currentPosition = enemy.body.getPosition();

    // figure the direction and distance
    sf::Vector2f direction = currentTarget - currentPosition;
    float distance         = std::sqrt(direction.x*direction.x + direction.y*direction.y);

    const float threshold = 5.0f;
    if (distance < threshold) {
        // advance
        enemy.waypointIndex++;

        // if we don't need to advance, attack
        if (enemy.waypointIndex >= waypoints.size() - 1) {
            enemy.startAttacking();
            return true;
        }

        // compute new target
        currentTarget = waypoints[enemy.waypointIndex + 1];
        direction     = currentTarget - currentPosition;
        distance      = std::sqrt(direction.x*direction.x + direction.y*direction.y);
    }

    // move
    if (distance > 0) {
        direction /= distance;
        sf::Vector2f movement = direction * enemy.movementSpeed * deltaTime;
        enemy.body.move(movement);
        enemy.healthBar.move(movement);
    }

    return false;
}
