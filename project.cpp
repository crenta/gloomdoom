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

    PlayerBase(const sf::Texture& texture) : health(1000) {
        shape.setTexture(texture);
        shape.setPosition(1920 - shape.getGlobalBounds().width, 300 - shape.getGlobalBounds().height + 20);

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
        healthBar.setSize(sf::Vector2f(100 * healthRatio, 10));
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
    : isDead(true), isAttacking(false), health(50), base(basePtr), waypointIndex(0), attackTimer(0.0f) {
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
        health = 50;
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
        healthBar.setSize(sf::Vector2f(40 * healthRatio, 5));
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
        attackRange = 48.0f;
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
    };

    std::vector<Wave> waves;
    size_t currentWave;
    float waveTimer;
    float currentInterval;
    float currentStagger;
    int enemiesSpawnedInWave;

    WaveManager() {
        waves.push_back({3, 0.0f, 0.2f});
        waves.push_back({5, 3.0f, 0.1f});
        waves.push_back({8, 5.0f, 0.3f});
        currentWave = 0;
        waveTimer = 0.0f;
        currentInterval = waves[0].initialInterval;
        currentStagger = waves[0].stagger;
        enemiesSpawnedInWave = 0;
    }

    bool isWaveComplete() const {
        return enemiesSpawnedInWave >= waves[currentWave].count;
    }

    void update(float deltaTime, std::vector<Enemy>& enemies, int& nextEnemyIndex, sf::Texture& enemyTexture, PathManager& pathManager) {
        if (currentWave >= waves.size()) return;

        waveTimer += deltaTime;
        if (waveTimer >= currentInterval && !isWaveComplete()) {
            int enemiesToSpawn = (currentWave >= 2) ? 2 : 1;
            float speed = calculateSpeed(currentWave);  //Speed based on the wave

            for (int i = 0; i < enemiesToSpawn && nextEnemyIndex < enemies.size(); i++) {
                enemies[nextEnemyIndex].activate(pathManager.getStartPoint(), speed);
                nextEnemyIndex++;
                enemiesSpawnedInWave++;
            }

            waveTimer = 0.0f; // Reset the timer
            currentInterval = waves[currentWave].initialInterval; // Prepare the interval for the next wave
            currentStagger = waves[currentWave].stagger;
        }

        if (isWaveComplete()) {
            if (++currentWave < waves.size()) { // Move to the next wave if available
                enemiesSpawnedInWave = 0;
                currentInterval = waves[currentWave].initialInterval;
                currentStagger = waves[currentWave].stagger;
                waveTimer = 0.0f; // Reset the timer for new wave
            }
        }
    }

    float calculateSpeed(size_t waveIndex) {
        return 150.0f + 2.0f * waveIndex; //formula to increase speed with the wave index
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Tower Defense Game");
    window.setFramerateLimit(60);

    sf::Texture mapTexture, enemyTexture, towerTexture, baseTexture, tumbleweedTexture, birdTexture, 
                rockTexture, treeTexture, flowerFirstTexture, flowerSecondTexture, flowerThirdTexture;
    if (!mapTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\map.png") ||
        !enemyTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\balloon.png") ||
        !towerTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\tower.png") ||
        !baseTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\base.png") || 
        !tumbleweedTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\tumbleweedspritesheet.png") ||
        !birdTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\birdtosize.png") ||
        !rockTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\rock.png") ||
        !treeTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\tree.png") ||
        !flowerFirstTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\flowerfirst.png") ||
        !flowerSecondTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\flowersecond.png") ||
        !flowerThirdTexture.loadFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\flowerthird.png")) {
        std::cerr << "Failed to load one or more textures" << std::endl;
        return EXIT_FAILURE;
    }

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("C:\\Users\\C\\Desktop\\GD4\\sprites\\GameMusic.wav")) {
        std::cerr << "Failed to load background music" << std::endl;
        return EXIT_FAILURE;
    }

    backgroundMusic.setLoop(true);  // Set the music to loop
    backgroundMusic.play();         // Start playing the music

    sf::Sprite mapSprite(mapTexture);
    PlayerBase base(baseTexture);

    // Environment objects
    sf::Sprite rock1(rockTexture), rock2(rockTexture),
                rock3(rockTexture), rock4(rockTexture),
                rock5(rockTexture), rock6(rockTexture),
                rock7(rockTexture), rock8(rockTexture),
                rock9(rockTexture), rock10(rockTexture),
                rock11(rockTexture), rock12(rockTexture),
                rock13(rockTexture), rock14(rockTexture);

    rock1.setPosition(200, 500);
    rock2.setPosition(400, 800);
    rock3.setPosition(240, 540);
    rock4.setPosition(260, 560);
    rock5.setPosition(280, 580);
    rock6.setPosition(300, 600);
    rock7.setPosition(320, 620);
    rock8.setPosition(340, 640);
    rock9.setPosition(360, 660);
    rock10.setPosition(380, 680);
    rock11.setPosition(400, 700);
    rock12.setPosition(420, 720);
    rock13.setPosition(1400, 600);  //good placement
    rock14.setPosition(1600, 700);  //good placement

    sf::Sprite tree1(treeTexture), tree2(treeTexture), tree3(treeTexture), tree4(treeTexture), tree5(treeTexture), tree6(treeTexture),
                tree7(treeTexture), tree8(treeTexture), tree9(treeTexture), tree10(treeTexture);
    
    tree1.setPosition(1000, 800);
    tree2.setPosition(1020, 820);
    tree3.setPosition(1040, 840);
    tree4.setPosition(1060, 860);
    tree5.setPosition(1080, 880);
    tree6.setPosition(1700, 300);
    tree7.setPosition(1000, 800);
    tree8.setPosition(1700, 300);
    tree9.setPosition(1000, 800);
    tree10.setPosition(1700, 300);

    //first flower
    sf::Sprite flowerFirst1(flowerFirstTexture), flowerFirst2(flowerFirstTexture), flowerFirst3(flowerFirstTexture),
                flowerFirst4(flowerFirstTexture), flowerFirst5(flowerFirstTexture), flowerFirst6(flowerFirstTexture),
                flowerFirst7(flowerFirstTexture), flowerFirst8(flowerFirstTexture), flowerFirst9(flowerFirstTexture);
    
    flowerFirst1.setPosition(500, 400);
    flowerFirst2.setPosition(520, 420);
    flowerFirst3.setPosition(540, 580);
    flowerFirst4.setPosition(560, 600);
    flowerFirst5.setPosition(550, 420);
    flowerFirst6.setPosition(600, 430);
    flowerFirst7.setPosition(500, 400);
    flowerFirst8.setPosition(550, 420);
    flowerFirst9.setPosition(600, 430);

    sf::Sprite flowerSecond1(flowerSecondTexture), flowerSecond2(flowerSecondTexture), flowerSecond3(flowerSecondTexture), flowerSecond4(flowerSecondTexture),
                flowerSecond5(flowerSecondTexture), flowerSecond6(flowerSecondTexture), flowerSecond7(flowerSecondTexture), flowerSecond8(flowerSecondTexture);
    
    flowerSecond1.setPosition(400, 450);
    flowerSecond2.setPosition(420, 470);
    flowerSecond1.setPosition(380, 430);
    flowerSecond2.setPosition(80, 80);
    flowerSecond1.setPosition(350, 390);
    flowerSecond2.setPosition(450, 475);
    flowerSecond1.setPosition(400, 450);
    flowerSecond2.setPosition(450, 475);

    sf::Sprite flowerThird1(flowerThirdTexture), flowerThird2(flowerThirdTexture), flowerThird3(flowerThirdTexture), flowerThird4(flowerThirdTexture),
                flowerThird5(flowerThirdTexture), flowerThird6(flowerThirdTexture), flowerThird7(flowerThirdTexture), flowerThird8(flowerThirdTexture);
    
    flowerThird1.setPosition(20, 20);
    flowerThird2.setPosition(40, 40);
    flowerThird3.setPosition(60, 60);
    flowerThird4.setPosition(70, 70);
    flowerThird5.setPosition(80, 80);
    flowerThird6.setPosition(90, 90);
    flowerThird7.setPosition(100, 100);
    flowerThird8.setPosition(120, 120);

    // Add all sprites to a vector
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
    const size_t maxTowers = 13;

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

        // Update game logic
        waveManager.update(deltaTime, enemies, nextEnemyIndex, enemyTexture, pathManager);
        for (auto& enemy : enemies) {
            if (!enemy.isDead) {
                pathManager.updatePosition(enemy, deltaTime);

                if (enemy.isAttacking) {  // Only update attack timers for attacking enemies
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

    elapsedTime += deltaTime;
    if (elapsedTime >= frameSwitchTime) {
            frameIndex = (frameIndex + 1) % 4;
            tumbleweedSprite.setTextureRect(sf::IntRect(frameIndex * 100, 0, 100, 100));
            tumbleweedSprite2.setTextureRect(sf::IntRect(frameIndex * 100, 0, 100, 100));
            elapsedTime = 0.0f;
    }

    // Update and move first tumbleweed
    tumbleweedPosition.x += tumbleweedSpeed * deltaTime;
    if (tumbleweedPosition.x < -100) {  // Check if it's completely off the left side of the screen
        tumbleweedPosition.x = 1920 + 100;  // Reset to just off the right side of the screen
    }
    tumbleweedSprite.setPosition(tumbleweedPosition);

        // Update and move the second tumbleweed
        tumbleweedPosition2.x += tumbleweedSpeed2 * deltaTime;
        if (tumbleweedPosition2.x > 1920 + 100) {  // Check if it's completely off the right side of the screen
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

        // Draw all static objects -- above map
        for (const auto& object : staticObjects) {
            window.draw(object);
        }

        //this will allow tumbleweeds to pass underneath base animation -- drawn before base (same as above)
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

        window.draw(birdSprite);
        window.draw(birdSprite2);
        window.display();
    }

    return 0;
}
