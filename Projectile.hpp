#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

class Projectile {
public:
    // texture, damage, and speed
    Projectile(const sf::Texture& texture, float damage, float speed);

    // launch it towards the target
    void launch(const sf::Vector2f& start, const sf::Vector2f& target);

    // update its position
    void update(float deltaTime);

    // get the sprite
    const sf::Sprite& getSprite() const;

    // activate
    bool isActive() const;

    // damage
    float getDamage() const;

    // deactivate
    void deactivate();

private:
    sf::Sprite mSprite;
    sf::Vector2f mDirection;
    float mSpeed;
    float mDamage;
    bool mActive;
};
