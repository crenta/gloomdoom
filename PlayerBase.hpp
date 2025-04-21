#pragma once

#include <SFML/Graphics.hpp>

class PlayerBase {
public:
    sf::Sprite shape;
    sf::RectangleShape healthBar;
    const int maxHealth = 1000; 
    int health;

    // texture for the base
    PlayerBase(const sf::Texture& texture); 

    // when the base takes damage
    void takeDamage(int damage);

    // update the health bar
    void updateHealthBar();

    // draw the base and the health bar
    void draw(sf::RenderWindow& window); 

    // is the base destroyed?
    bool isDestroyed() const;
};