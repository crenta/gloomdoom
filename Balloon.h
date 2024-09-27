//Balloon.h
#pragma once
#include <SFML/Graphics.hpp>

class Balloon {
public:
    Balloon(const sf::Texture& texture) {
        sprite.setTexture(texture);
    }

    void update(const sf::Vector2f& target, float dt) {
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
private:
    sf::Sprite sprite;
};
