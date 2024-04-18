#include "Utility.h"
#include <cmath> // Include for sqrt and pow functions

// Calculate the velocity needed to move from start to end at a given speed
sf::Vector2f calculateVelocity(const sf::Vector2f &start, const sf::Vector2f &end, float speed)
{
    sf::Vector2f direction = end - start;

    // Check for zero length to avoid divide by zero
    if (direction.x == 0.f && direction.y == 0.f)
    {
        return sf::Vector2f(0.f, 0.f);
    }

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length; // Normalize

    return direction * speed;
}
