// GameEntities.h
#pragma once
#include <SFML/Graphics.hpp>

class Enemy {
public:
    void move();
    sf::CircleShape body;
};

class Tower : public sf::Drawable, public sf::Transformable {
public:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class PlayerBase {
public:
    int baseHealth;
};
