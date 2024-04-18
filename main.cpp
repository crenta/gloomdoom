// main.cpp
#include "GameStateManager.h"
#include <SFML /

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Gloom Doom: Chlorophyll Carnage");
    window.setFramerateLimit(60);

    GameStateManager gameManager;
    gameManager.startNewLevel(1); // Start game at level 1

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            gameManager.handleEvent(event);
        }

        gameManager.refresh();
        window.clear();
        gameManager.graphicRendering(window);
        window.display();
    }

    return 0;
}