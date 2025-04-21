#include "Tower.hpp"
#include "Enemy.hpp"
#include <algorithm>

Tower::Tower(const sf::Texture& texture) {
    // initialize the tower
    shape.setTexture(texture);
    sf::FloatRect b = shape.getLocalBounds();
    shape.setOrigin(b.width/2.f, b.height/2.f);

    // health bar setup
    healthBar.setSize({ b.width, 5.f });
    healthBar.setFillColor(sf::Color::Green);
    healthBar.setOutlineColor(sf::Color::Black);
    healthBar.setOutlineThickness(1.f);
}

// attack enemies
bool Tower::attackEnemy(Enemy& enemy, float deltaTime) {
    // timer for firing
    fireAccumulator += deltaTime;

    // only fire on the interval
    if (fireAccumulator >= attackCooldown
        && !enemy.isDead
        && isInRange(enemy.body.getPosition()))
    {
        // fire once
        fireAccumulator = 0.f;
        return true;
    }
    return false;
}

// calculate if the enemy is in range
bool Tower::isInRange(const sf::Vector2f& enemyPos) const {
    sf::Vector2f d = shape.getPosition() - enemyPos;
    return (d.x*d.x + d.y*d.y) <= (attackRange * attackRange);
}

// draw the tower and the health bar
void Tower::draw(sf::RenderWindow& window) {
    sf::Vector2f p = shape.getPosition();
    float halfH = shape.getLocalBounds().height/2.f;
    healthBar.setPosition(p.x - healthBar.getSize().x/2.f,
                          p.y - halfH - 8.f);

    window.draw(healthBar);
    window.draw(shape);
}

// take damage and update the health bar
void Tower::takeDamage(int dmg) {
    health = std::max(0, health - dmg);

    float ratio = float(health) / float(maxHealth);
    healthBar.setSize({ shape.getLocalBounds().width * ratio, 5.f });

    // colors for health range
    if (ratio < 0.3f)       healthBar.setFillColor(sf::Color::Red);
    else if (ratio < 0.6f)  healthBar.setFillColor(sf::Color::Yellow);
    else                    healthBar.setFillColor(sf::Color::Green);
}
