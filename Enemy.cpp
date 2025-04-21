#include "Enemy.hpp"
#include "PathManager.hpp"
#include "PlayerBase.hpp"
#include "Tower.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

// offscreen starting position of enemies
const sf::Vector2f Enemy::OFF_SCREEN_POS(-1000.f, -1000.f);

Enemy::Enemy(const sf::Texture& texture, PlayerBase* basePtr)
: base(basePtr)
{
    body.setTexture(texture);
    body.setScale(0.5f, 0.5f);
    auto b = body.getLocalBounds();
    body.setOrigin(b.width/2.f, b.height/2.f);
    body.setPosition(OFF_SCREEN_POS);

    health = maxHealth = 50;
    healthBar.setSize({40.f,5.f});
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setOrigin(20.f,0.f);
    healthBar.setPosition(OFF_SCREEN_POS);
}

// --- activate the enemy with default characteristics
void Enemy::activate(sf::Vector2f startPosition, float speed) {
    body.setPosition(startPosition);
    healthBar.setPosition(startPosition.x, startPosition.y - 30.f);
    movementSpeed = speed;
    isDead         = false;
    isAttacking    = false;
    isAttackingTower = false;
    health         = maxHealth;
    waypointIndex  = 0;
    attackAccumulator = 0.0f;
    updateHealthBar();
}

// --------- take damage & Die --------------
void Enemy::takeDamage(int dmg) {
    if (isDead) return;
    health = std::max(0, health - dmg);
    aggroed = true;
    updateHealthBar();
    if (health == 0) kill();
}

// ---------- enemy death -------------------
void Enemy::kill() {
    isDead = true;
    isAttacking = false;
    body.setPosition(OFF_SCREEN_POS);
    healthBar.setPosition(OFF_SCREEN_POS);
    std::cout << "Enemy killed!\n";
}

// --------- movement along hardcoded path ---------
void Enemy::updateMovement(float deltaTime,
                           PathManager& pathManager,
                           const std::vector<Enemy>& allEnemies)
{
    // if dead or attacking, return
    if (isDead || isAttacking)
        return;

    // format the pack to follow the leader (used after tower death)
    bool isLeader = true;
    for (const auto& other : allEnemies) {
        if (&other == this || other.isDead)
            continue;
        if (other.waypointIndex > waypointIndex) {
            isLeader = false;
            break;
        }
    }

    // the leader will always attack
    if (isLeader) {
        bool reachedEnd = pathManager.updatePosition(*this, deltaTime);
        if (reachedEnd)
            startAttacking();
        return;
    }

    // format the spaceing of enemies so they dont stack
    constexpr float MIN_SPACING = 30.f;
    float minDist2 = MIN_SPACING * MIN_SPACING;

    for (const auto& other : allEnemies) {
        if (&other == this || other.isDead)
            continue;

        float dx = body.getPosition().x - other.body.getPosition().x;
        float dy = body.getPosition().y - other.body.getPosition().y;
        // if enemies are too close together skip movement on one
        if (dx*dx + dy*dy < minDist2) {
            return;
        }
    }

    // march along the path
    bool reachedEnd = pathManager.updatePosition(*this, deltaTime);
    if (reachedEnd)
        startAttacking();
}

// --- start attacking ---
void Enemy::startAttacking() {
    if (!isDead && !isAttacking) {
        isAttacking = true;
        attackAccumulator = 0.0f;
        std::cout << "Enemy starting attack!\n";
    }
}

// --- accumulate each attack for the timer ---
void Enemy::updateAttackTimer(float deltaTime) {
    if (!isDead && (isAttacking || isAttackingTower)) {
        attackAccumulator += deltaTime;
    }
}


// --- try to attack the tower in the interval ----
bool Enemy::tryAttackTower() {
    if (isAttackingTower
        && attackAccumulator >= ENEMY_ATTACK_INTERVAL
        && targetTower)
    {
        attackAccumulator -= ENEMY_ATTACK_INTERVAL;
        targetTower->takeDamage(ENEMY_ATTACK_DAMAGE);
        return true;
    }
    return false;
}


// --- try to hit the main base in the interval ----
bool Enemy::tryAttackBase() {
    if (isAttacking
        && attackAccumulator >= ENEMY_ATTACK_INTERVAL
        && base)
    {
        attackAccumulator -= ENEMY_ATTACK_INTERVAL;
        base->takeDamage(ENEMY_ATTACK_DAMAGE);
        return true;
    }
    return false;
}


// --- enemy health bar update ---
void Enemy::updateHealthBar() {
    float ratio = float(health) / float(maxHealth);
    healthBar.setSize({40.f * ratio, 5.f});
    auto pos = body.getPosition();
    healthBar.setPosition(pos.x, pos.y - 30.f);
}


// --- draw the enemy and the health bar
void Enemy::draw(sf::RenderWindow& window) {
    if (isDead)
        return;

    window.draw(body);

    // only draw health bar if enemy has taken damage
    if (health < maxHealth) {
        // compute health
        float ratio = float(health) / float(maxHealth);

        sf::FloatRect b = body.getGlobalBounds();

        healthBar.setSize({ b.width * ratio, 5.f });

        // center the health bar
        float barX = b.left + (b.width - b.width * ratio) * 0.5f;
        float barY = b.top  - 8.f;

        healthBar.setPosition(barX, barY);

        //draw new health bar
        window.draw(healthBar);
    }
}

