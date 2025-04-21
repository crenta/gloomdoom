#include "PlayerBase.hpp"
#include <iostream>
#include <algorithm>

PlayerBase::PlayerBase(const sf::Texture& texture) : health(maxHealth) {
    shape.setTexture(texture);
    // hardcoded position for now
    float targetX = 1750;
    float targetY = 300;
    sf::FloatRect bounds = shape.getLocalBounds();
    shape.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    shape.setPosition(targetX + bounds.width / 2.f + 10.f, targetY);

    // health bar for base
    healthBar.setSize(sf::Vector2f(100, 10));
    healthBar.setFillColor(sf::Color::Green);
    healthBar.setOrigin(healthBar.getSize().x / 2.f, healthBar.getSize().y / 2.f);
    healthBar.setPosition(shape.getPosition().x, shape.getPosition().y - bounds.height / 2.f - 15.f);

    std::cout << "PlayerBase constructed with health = " << health << std::endl;

}

// base takes damage
void PlayerBase::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        std::cout << "Base destroyed!" << std::endl; 
    }
    updateHealthBar();
}

// base health bar update
void PlayerBase::updateHealthBar() {
    float healthRatio = std::max(0.0f, static_cast<float>(health) / static_cast<float>(maxHealth));
    healthBar.setSize(sf::Vector2f(100 * healthRatio, 10));
}

// draw the base
void PlayerBase::draw(sf::RenderWindow& window) {
    window.draw(shape);
    // draw the health bar when the base takes damage
    if (health < maxHealth) {
         window.draw(healthBar);
    }
}

// check if the base is destroyed
bool PlayerBase::isDestroyed() const {
    return health <= 0;
}