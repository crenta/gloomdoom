#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "Balloon.h"

class Enemy;
class Tower;
class PlayerBase;
class WaveManager;
class PathManager;

class PathManager {
public:
    std::vector<sf::Vector2f> waypoints;

    PathManager() {
        waypoints = {
            sf::Vector2f(0, 540),
            sf::Vector2f(250, 540),
            sf::Vector2f(250, 300),
            sf::Vector2f(1750, 300)
        };
    }

    sf::Vector2f getStartPoint() const {
        return waypoints.front();
    }

    bool updatePosition(Enemy& enemy, float deltaTime);
};

class PlayerBase {
public:
    sf::Sprite shape;
    sf::RectangleShape healthBar;
    int health;

    PlayerBase(const sf::Texture& texture) : health(6000) {
        shape.setTexture(texture);
        shape.setPosition(1920 - shape.getGlobalBounds().width, 300 - shape.getGlobalBounds().height + 120);

        healthBar.setSize(sf::Vector2f(100, 10));
        healthBar.setFillColor(sf::Color::Green);
        healthBar.setPosition(shape.getPosition().x, shape.getPosition().y - 15);
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            health = 0;
            std::cout << "Base destroyed!" << std::endl;
        }
        updateHealthBar();
    }

    void updateHealthBar() {
        float healthRatio = static_cast<float>(health) / 1000.0f;  // Update ratio to max health
        healthBar.setSize(sf::Vector2f(100 * healthRatio/5, 10));
    }
};

class Enemy {
public:
    sf::Sprite body;
    sf::RectangleShape healthBar;
    bool isDead, isAttacking;
    float movementSpeed;
    int health;
    size_t waypointIndex;
    float attackTimer;
    PlayerBase* base;

    Enemy(const sf::Texture& texture, PlayerBase* basePtr) 
    : isDead(true), isAttacking(false), health(1000), base(basePtr), waypointIndex(0), attackTimer(0.0f) {
        body.setTexture(texture);
        body.setScale(0.5, 0.5);
        body.setPosition(-100, 540);
        healthBar.setSize(sf::Vector2f(40, 5));
        healthBar.setFillColor(sf::Color::Red);
        healthBar.setPosition(body.getPosition().x, body.getPosition().y - 10);
    }

    void activate(sf::Vector2f startPosition, float speed) {
        body.setPosition(startPosition);
        movementSpeed = speed;
        isDead = false;
        isAttacking = false;
        health = 1000;
        waypointIndex = 0;
        attackTimer = 0.0f;
        updateHealthBar();
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            kill();
        }
        updateHealthBar();
    }

    void startAttacking() {
        if (!isAttacking) {
            isAttacking = true;
            attackTimer = 0.0f;
        }
    }

    void updateHealthBar() {
        float healthRatio = std::max(0.0f, static_cast<float>(health) / 50.0f);
        healthBar.setSize(sf::Vector2f(40 * healthRatio/10, 5));
        healthBar.setPosition(body.getPosition().x, body.getPosition().y - 10);
    }

    void kill() {
        isDead = true;
        body.setPosition(-100, -100);
        healthBar.setPosition(-100, -100);
    }

    void attack() {
        if (isAttacking && attackTimer >= 1.0f && base) {
            base->takeDamage(5);
            attackTimer = 0.0f;  // Reset the timer after attack
        }
    }

    void updateAttackTimer(float deltaTime) {
        if (isAttacking) {
            attackTimer += deltaTime;
        }
    }
};
bool PathManager::updatePosition(Enemy& enemy, float deltaTime) {
    if (enemy.isDead || enemy.waypointIndex >= waypoints.size()) {
        return true; // Enemy stops moving if it has reached the end or is dead
    }

    if (enemy.waypointIndex == waypoints.size() - 1) {
        enemy.startAttacking();  // Call this method when enemy reaches the last waypoint
        return false;
    }

    sf::Vector2f& currentTarget = waypoints[enemy.waypointIndex + 1];
    sf::Vector2f direction = currentTarget - enemy.body.getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance > 0) {
        direction /= distance;
        enemy.body.move(direction * enemy.movementSpeed * deltaTime);
        enemy.healthBar.move(direction * enemy.movementSpeed * deltaTime);
    }

    if (distance < 5.0f) {
        enemy.waypointIndex++;
    }
    return false;
}

class Tower {
public:
    sf::Sprite shape;
    float attackRange;

    Tower(const sf::Texture& texture) {
        shape.setTexture(texture);
        shape.setOrigin(shape.getLocalBounds().width / 2, shape.getLocalBounds().height / 2);
        attackRange = 200.0f;
    }

    bool isInRange(sf::Vector2f enemyPos) const {
        sf::Vector2f center = shape.getPosition();
        float distance = std::sqrt(std::pow(center.x - enemyPos.x, 2) + std::pow(center.y - enemyPos.y, 2));
        return distance <= attackRange;
    }

    void attackEnemy(Enemy& enemy) {
        if (isInRange(enemy.body.getPosition()) && !enemy.isDead) {
            enemy.takeDamage(3); // Damage value can be adjusted
        }
    }
};


class WaveManager {
public:
    struct Wave {
        int count;
        float initialInterval;
        float stagger;
        float speedIncrease;
    };

    Wave currentWaveSettings;
    size_t currentWaveIndex;
    float waveTimer;
    float currentInterval;
    float currentStagger;
    int enemiesSpawnedInWave;

    WaveManager() : currentWaveIndex(0), waveTimer(0.0f), enemiesSpawnedInWave(0) {
        currentWaveSettings = {5, 1.1f, 0.2f, 5.0f}; // initial settings
        currentInterval = currentWaveSettings.initialInterval;
        currentStagger = currentWaveSettings.stagger;
    }

    bool isWaveComplete() const {
        return enemiesSpawnedInWave >= currentWaveSettings.count;
    }

    void update(float deltaTime, std::vector<Enemy>& enemies, int& nextEnemyIndex, sf::Texture& enemyTexture, PathManager& pathManager) {
        waveTimer += deltaTime;
        if (waveTimer >= currentInterval && !isWaveComplete()) {
            float speed = 150.0f + currentWaveSettings.speedIncrease * currentWaveIndex; // Speed based on the wave

            for (int i = 0; i < 1 && nextEnemyIndex < enemies.size(); i++) { // Spawn one enemy at a time
                enemies[nextEnemyIndex].activate(pathManager.getStartPoint(), speed);
                nextEnemyIndex++;
                enemiesSpawnedInWave++;
            }

            waveTimer = 0.0f; // Reset the timer
            currentInterval = currentWaveSettings.initialInterval; // Prepare the interval for the next wave
            currentStagger = currentWaveSettings.stagger;
        }

        if (isWaveComplete()) {
            currentWaveIndex++; // Move to the next wave
            currentWaveSettings.count += 2; // Increase enemy count for next wave
            currentWaveSettings.speedIncrease += 2.0f; // Increase speed increment for next wave
            enemiesSpawnedInWave = 0;
            waveTimer = 0.0f; // Reset the timer for new wave
        }
    }
};


int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Tower Defense Game");
    window.setFramerateLimit(60);

    bool gameOver = false;
    sf::Font gameFont;
    if (!gameFont.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\Jersey25-Regular.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return EXIT_FAILURE;
    }

    sf::Text gameOverText("Game Over!", gameFont, 200);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(950 - gameOverText.getGlobalBounds().width / 2, 500 - gameOverText.getGlobalBounds().height / 2);

    sf::Texture mapTexture, enemyTexture, towerTexture, baseTexture, tumbleweedTexture, birdTexture, 
                rockTexture, treeTexture, flowerFirstTexture, flowerSecondTexture, flowerThirdTexture;
    if (!mapTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\background.png") ||
        !enemyTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\enemy.png") ||
        !towerTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\tower.png") ||
        !baseTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\base.png") ||
        !tumbleweedTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\tumbleweedspritesheet.png") ||
        !birdTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\birdtosize.png") ||
        !rockTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\rock.png") ||
        !treeTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\tree.png") ||
        !flowerFirstTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\flowerfirst.png") ||
        !flowerSecondTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\flowersecond.png") ||
        !flowerThirdTexture.loadFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD5\\sprites\\flowerthird.png")) {
        std::cerr << "Failed to load one or more textures" << std::endl;
        return EXIT_FAILURE;
    }

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("C:\\Users\\C\\Desktop\\Codes\\C++\\GloomDoom\\GD4\\sprites\\GameMusic.wav")) {
        std::cerr << "Failed to load background music" << std::endl;
        return EXIT_FAILURE;
    }

    backgroundMusic.setLoop(true);  // Set the music to loop
    backgroundMusic.play();         // Start playing the music

    sf::Sprite mapSprite(mapTexture);
    PlayerBase base(baseTexture);

    // Environment objects
    // Environment objects
    sf::Sprite rock1(rockTexture), rock2(rockTexture),
        rock3(rockTexture), rock4(rockTexture),
        rock5(rockTexture), rock6(rockTexture),
        rock7(rockTexture), rock8(rockTexture),
        rock9(rockTexture), rock10(rockTexture),
        rock11(rockTexture), rock12(rockTexture),
        rock13(rockTexture), rock14(rockTexture);

    rock1.setPosition(1600, 100);
    rock1.setScale(2.5, 2.5);
    rock2.setPosition(1100, 30);
    rock2.setScale(2.5, 2.5);
    rock3.setPosition(700, 120);
    rock3.setScale(2.5, 2.5);
    rock4.setPosition(300, 60);
    rock4.setScale(2.5, 2.5);
    rock5.setPosition(70, 400);
    rock5.setScale(2.5, 2.5);
    rock6.setPosition(600, 600);
    rock6.setScale(2.5, 2.5);
    rock7.setPosition(70, 900);
    rock7.setScale(2.5, 2.5);
    rock8.setPosition(400, 850);
    rock8.setScale(2.5, 2.5);
    rock9.setPosition(700, 500);
    rock9.setScale(2.5, 2.5);
    rock10.setPosition(850, 680);
    rock10.setScale(2.5, 2.5);
    rock11.setPosition(900, 1000);
    rock11.setScale(2.5, 2.5);
    rock12.setPosition(1110, 1200);
    rock12.setScale(2.5, 2.5);
    rock13.setPosition(1400, 600);
    rock13.setScale(2.5, 2.5);
    rock14.setPosition(1600, 700);
    rock14.setScale(2.5, 2.5);

    sf::Sprite tree1(treeTexture), tree2(treeTexture), tree3(treeTexture), tree4(treeTexture), tree5(treeTexture), tree6(treeTexture),
        tree7(treeTexture), tree8(treeTexture), tree9(treeTexture), tree10(treeTexture);

    tree1.setPosition(1400, 700);
    tree1.setScale(3.0, 3.0);
    tree2.setPosition(1550, 60);
    tree2.setScale(3.0, 3.0);
    tree3.setPosition(410, 12);
    tree3.setScale(3.0, 3.0);
    tree4.setPosition(30, 635);
    tree4.setScale(3.0, 3.0);
    tree5.setPosition(1080, 500);
    tree5.setScale(3.0, 3.0);
    tree6.setPosition(700, 450);
    tree6.setScale(3.0, 3.0);
    tree7.setPosition(900, 800);
    tree7.setScale(3.0, 3.0);
    tree8.setPosition(475, 750);
    tree8.setScale(3.0, 3.0);
    tree9.setPosition(820, 68);
    tree9.setScale(3.0, 3.0);
    tree10.setPosition(1600, 400);
    tree10.setScale(3.0, 3.0);

    // first flower
    sf::Sprite flowerFirst1(flowerFirstTexture), flowerFirst2(flowerFirstTexture), flowerFirst3(flowerFirstTexture),
        flowerFirst4(flowerFirstTexture), flowerFirst5(flowerFirstTexture), flowerFirst6(flowerFirstTexture),
        flowerFirst7(flowerFirstTexture), flowerFirst8(flowerFirstTexture), flowerFirst9(flowerFirstTexture);

    flowerFirst1.setPosition(1700, 900);
    flowerFirst1.setScale(2.0, 2.0);
    flowerFirst2.setPosition(1750, 650);
    flowerFirst2.setScale(2.0, 2.0);
    flowerFirst3.setPosition(1300, 150);
    flowerFirst3.setScale(2.0, 2.0);
    flowerFirst4.setPosition(930, 600);
    flowerFirst4.setScale(2.0, 2.0);
    flowerFirst5.setPosition(1100, 800);
    flowerFirst5.setScale(2.0, 2.0);
    flowerFirst6.setPosition(475, 525);
    flowerFirst6.setScale(2.0, 2.0);
    flowerFirst7.setPosition(300, 900);
    flowerFirst7.setScale(2.0, 2.0);
    flowerFirst8.setPosition(60, 300);
    flowerFirst8.setScale(2.0, 2.0);
    flowerFirst9.setPosition(100, 10);
    flowerFirst9.setScale(2.0, 2.0);

    sf::Sprite flowerSecond1(flowerSecondTexture), flowerSecond2(flowerSecondTexture), flowerSecond3(flowerSecondTexture), flowerSecond4(flowerSecondTexture),
        flowerSecond5(flowerSecondTexture), flowerSecond6(flowerSecondTexture), flowerSecond7(flowerSecondTexture), flowerSecond8(flowerSecondTexture);

    flowerSecond1.setPosition(450, 650); // good position, on bottom
    flowerSecond1.setScale(3.0, 3.0);
    flowerSecond2.setPosition(1000, 475); // good position, on bottom
    flowerSecond2.setScale(3.0, 3.0);

    sf::Sprite flowerThird1(flowerThirdTexture), flowerThird2(flowerThirdTexture), flowerThird3(flowerThirdTexture), flowerThird4(flowerThirdTexture),
        flowerThird5(flowerThirdTexture), flowerThird6(flowerThirdTexture), flowerThird7(flowerThirdTexture), flowerThird8(flowerThirdTexture);

    flowerThird1.setPosition(300, 750);
    flowerThird1.setScale(2.5, 2.5);
    flowerThird2.setPosition(650, 50);
    flowerThird2.setScale(2.5, 2.5);
    flowerThird3.setPosition(750, 700);
    flowerThird3.setScale(2.5, 2.5);
    flowerThird4.setPosition(1200, 800);
    flowerThird4.setScale(2.5, 2.5);
    flowerThird5.setPosition(1400, 500);
    flowerThird5.setScale(2.5, 2.5);
    flowerThird6.setPosition(1400, 50);
    flowerThird6.setScale(2.5, 2.5);
    flowerThird7.setPosition(1100, 150);
    flowerThird7.setScale(2.5, 2.5);
    flowerThird8.setPosition(150, 120);
    flowerThird8.setScale(2.5, 2.5);

    // Add all sprites to a vector for easy management
    std::vector<sf::Sprite> staticObjects{
        rock1, rock2, rock3, rock4, rock5, rock6, rock7, rock8, rock9, rock10, rock11, rock12, rock13, rock14,
        tree1, tree2, tree3, tree4, tree5, tree6, tree7, tree8, tree9, tree10,
        
        flowerFirst1, flowerFirst2, flowerFirst3, flowerFirst4, flowerFirst5, flowerFirst6, flowerFirst7, flowerFirst8, flowerFirst9,

        flowerSecond1, flowerSecond2, flowerSecond3, flowerSecond4, flowerSecond5, flowerSecond6, flowerSecond7, flowerSecond8,

        flowerThird1, flowerThird2, flowerThird3, flowerThird4, flowerThird5, flowerThird6, flowerThird7, flowerThird8
    };

    std::vector<Enemy> enemies;
    for (int i = 0; i < 60; i++) {
        enemies.emplace_back(enemyTexture, &base);
    }

    std::vector<Tower> towers;
    int maxTowers = 10;

// Tumbleweed and bird animations setup
sf::Sprite tumbleweedSprite(tumbleweedTexture), tumbleweedSprite2(tumbleweedTexture);
sf::Sprite birdSprite(birdTexture), birdSprite2(birdTexture);

tumbleweedSprite.setTextureRect(sf::IntRect(0, 0, 100, 100));
tumbleweedSprite2.setTextureRect(sf::IntRect(0, 0, 100, 100));
birdSprite.setTextureRect(sf::IntRect(0, 0, 135, 92));
birdSprite2.setTextureRect(sf::IntRect(135, 0, 135, 92));

// Flip the second tumbleweed and bird to face the opposite direction
tumbleweedSprite2.setScale(1.0f, 1.0f);  // Flip horizontally
birdSprite2.setScale(-1.0f, 1.0f);        // Flip horizontally

float tumbleweedSpeed = -200.0f, tumbleweedSpeed2 = 200.0f;
float birdSpeed = -200.0f, birdSpeed2 = 200.0f;

sf::Vector2f tumbleweedPosition(1920, 800), tumbleweedPosition2(20, 200);
sf::Vector2f birdPosition(1920, 50), birdPosition2(-135, 700); 

float elapsedTime = 0.0f, birdAnimationTime = 0.0f;
const float frameSwitchTime = 0.2f, birdFrameSwitchTime = 0.1f;
int frameIndex = 0, birdFrameIndex = 0;

    WaveManager waveManager;
    PathManager pathManager;

    int nextEnemyIndex = 0;

    sf::Clock gameClock;
    float deltaTime;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left && towers.size() < maxTowers) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    Tower newTower(towerTexture);
                    newTower.shape.setPosition(mousePos);
                    towers.push_back(newTower);
                }
            }
        }
        
        deltaTime = gameClock.restart().asSeconds();

        if (!gameOver) {
            waveManager.update(deltaTime, enemies, nextEnemyIndex, enemyTexture, pathManager);
            for (auto& enemy : enemies) {
                if (!enemy.isDead) {
                    pathManager.updatePosition(enemy, deltaTime);

                    if (enemy.isAttacking) {
                        enemy.updateAttackTimer(deltaTime);
                        enemy.attack();
                    }

                    for (auto& tower : towers) {
                        tower.attackEnemy(enemy);
                    }

                    if (enemy.body.getGlobalBounds().intersects(base.shape.getGlobalBounds())) {
                        base.takeDamage(3);
                    }
                }
            }

            if (base.health <= 0) {
                gameOver = true;
                backgroundMusic.stop();  // Optional: Stop music on game over
            }
        }

        // Update and move first tumbleweed
        elapsedTime += deltaTime;
        if (elapsedTime >= frameSwitchTime) {
            frameIndex = (frameIndex + 1) % 4;
            tumbleweedSprite.setTextureRect(sf::IntRect(frameIndex * 100, 0, 100, 100));
            tumbleweedSprite2.setTextureRect(sf::IntRect(frameIndex * 100, 0, 100, 100));
            elapsedTime = 0.0f;
        }

        tumbleweedPosition.x += tumbleweedSpeed * deltaTime;
        if (tumbleweedPosition.x < -100) {
            tumbleweedPosition.x = 1920 + 100;  // Reset to just off the right side of the screen
        }
        tumbleweedSprite.setPosition(tumbleweedPosition);

        tumbleweedPosition2.x += tumbleweedSpeed2 * deltaTime;
        if (tumbleweedPosition2.x > 1920 + 100) {
            tumbleweedPosition2.x = -100;  // Reset to just off the left side of the screen
        }
        tumbleweedSprite2.setPosition(tumbleweedPosition2);

        // Birds
        birdAnimationTime += deltaTime;
        if (birdAnimationTime >= birdFrameSwitchTime) {
            birdFrameIndex = (birdFrameIndex + 1) % 2;
            birdSprite.setTextureRect(sf::IntRect(birdFrameIndex * 135, 0, 135, 92));
            birdSprite2.setTextureRect(sf::IntRect(birdFrameIndex * 135, 0, 135, 92));
            birdAnimationTime = 0.0f;
        }

        birdPosition.x += birdSpeed * deltaTime;
        if (birdPosition.x < -135) {
            birdPosition.x = 1920;
        }
        birdSprite.setPosition(birdPosition);

        birdPosition2.x += birdSpeed2 * deltaTime;
        if (birdPosition2.x > 1920 + 135) {
            birdPosition2.x = -135;
        }
        birdSprite2.setPosition(birdPosition2);

        window.clear();
        window.draw(mapSprite);

        // Draw all static and dynamic game objects
        for (const auto& object : staticObjects) {
            window.draw(object);
        }

        if (!gameOver) {
             window.draw(birdSprite);
            window.draw(birdSprite2);
            window.draw(tumbleweedSprite);
            window.draw(tumbleweedSprite2);
            window.draw(base.shape);
            window.draw(base.healthBar);
            for (auto& tower : towers) {
                window.draw(tower.shape);
            }
            for (auto& enemy : enemies) {
                if (!enemy.isDead) {
                    window.draw(enemy.body);
                    window.draw(enemy.healthBar);
                }
            }
        } else {
            window.draw(gameOverText);
        }
        window.display();
    }
    
    return 0;
}
