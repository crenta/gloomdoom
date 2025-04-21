#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class PlayerBase;
class Tower;
class PathManager;

class Enemy {
public:
    Enemy(const sf::Texture& texture, PlayerBase* basePtr);

    // used by wave manager to activate enemies
    void activate(sf::Vector2f startPosition, float speed);

    // damage & death
    void takeDamage(int damage);
    void kill();

    // once you reach the end of the path...
    void startAttacking();

    // movement along the path
    void updateMovement(float deltaTime,
        PathManager& pathManager,
        const std::vector<Enemy>& allEnemies);

    // accumulator to calculate the next attack
    void updateAttackTimer(float deltaTime);

    // should happen once per interval
    bool tryAttackTower();
    bool tryAttackBase();

    // update health bars after damage
    void updateHealthBar();

    // draw the game and updates
    void draw(sf::RenderWindow& window);

    // public stuff
    sf::Sprite        body;
    sf::RectangleShape healthBar;
    bool              isDead         = true;
    bool              isAttacking    = false;
    bool              isAttackingTower = false;
    Tower*            targetTower    = nullptr;
    float             movementSpeed  = 0.0f;
    int               health         = 0;
    int               maxHealth      = 0;
    size_t            waypointIndex  = 0;
    bool aggroed = false;

private:
    PlayerBase* base = nullptr;

    // accumulator for tower and main base
    float attackAccumulator = 0.0f;

    // this defines how often the enemy attacks
    static constexpr float ENEMY_ATTACK_INTERVAL = 1.0f;

    // how much damage the enemy deals per hit
    static constexpr int   ENEMY_ATTACK_DAMAGE   = 5;

    // off screen position
    static const sf::Vector2f OFF_SCREEN_POS;
};
