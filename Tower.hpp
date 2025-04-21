#pragma once
#include <SFML/Graphics.hpp>

class Enemy;

class Tower {
public:
    Tower(const sf::Texture& texture);

    bool attackEnemy(Enemy& enemy, float deltaTime);

    bool isInRange(const sf::Vector2f& enemyPos) const;

    void draw(sf::RenderWindow& window);

    void takeDamage(int dmg);

    // public
    sf::Sprite shape;
    float attackRange    = 250.f;  // attack range
    float attackCooldown = 0.75f;  // time between attacks
    int   damage         = 3;
    bool  isAlive() const { return health > 0; }

private:
    float fireAccumulator = 0.0f;
    int   health          = 25;
    int   maxHealth       = 25;

    sf::RectangleShape healthBar;
};
