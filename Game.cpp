#include "Game.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include <iomanip>

// --- main constructor ---
Game::Game()
:   mWindow(sf::VideoMode(1920, 1080), "Tower Defense Game"), // create the game window
    // initialize stuff
    mPlayerBase(nullptr),
    mNextEnemyIndex(0),
    mMaxTowers(13)
{
    mWindow.setFramerateLimit(60);

    // load resources and catch errors
    try {
        loadTextures();
        loadSounds();
    } catch (const std::runtime_error& e) {
        std::cerr << "Error loading resources: " << e.what() << std::endl;
        return;
    }

    // initialize objects that require textures
    mPlayerBase = new PlayerBase(mBaseTexture);
    mMapSprite.setTexture(mMapTexture);

    // setup initial state
    setupInitialState();
    setupAnimations();


    // --- populate the enemy pool ---
    const int totalEnemies = 10;
    mEnemies.reserve(totalEnemies);
    for (int i = 0; i < totalEnemies; ++i) {
        mEnemies.push_back(Enemy(mEnemyTexture, mPlayerBase));
    }
    std::cout << "Enemy pool populated with " << totalEnemies << " enemies." << std::endl;

    // configure the background music
    mBackgroundMusic.setLoop(true);
    mBackgroundMusic.play();
}

// --- destructor ---
Game::~Game() {
    delete mPlayerBase;
    mPlayerBase = nullptr;
}

// --- loading resources ---
void Game::loadTextures() {
    std::string pathPrefix = "sprites/";
    if (!mMapTexture.loadFromFile(pathPrefix + "map.png")) throw std::runtime_error("Failed to load map.png");
    if (!mEnemyTexture.loadFromFile(pathPrefix + "balloon.png")) throw std::runtime_error("Failed to load balloon.png");
    if (!mTowerTexture.loadFromFile(pathPrefix + "tower.png")) throw std::runtime_error("Failed to load tower.png");
    if (!mBaseTexture.loadFromFile(pathPrefix + "base.png")) throw std::runtime_error("Failed to load base.png");
    if (!mTumbleweedTexture.loadFromFile(pathPrefix + "tumbleweedspritesheet.png")) throw std::runtime_error("Failed to load tumbleweedspritesheet.png");
    if (!mBirdTexture.loadFromFile(pathPrefix + "birdtosize.png")) throw std::runtime_error("Failed to load birdtosize.png");
    if (!mRockTexture.loadFromFile(pathPrefix + "rock.png")) throw std::runtime_error("Failed to load rock.png");
    if (!mTreeTexture.loadFromFile(pathPrefix + "tree.png")) throw std::runtime_error("Failed to load tree.png");
    if (!mFlowerFirstTexture.loadFromFile(pathPrefix + "flowerfirst.png")) throw std::runtime_error("Failed to load flowerfirst.png");
    if (!mFlowerSecondTexture.loadFromFile(pathPrefix + "flowersecond.png")) throw std::runtime_error("Failed to load flowersecond.png");
    if (!mFlowerThirdTexture.loadFromFile(pathPrefix + "flowerthird.png")) throw std::runtime_error("Failed to load flowerthird.png");
    if (!mProjectileTexture.loadFromFile(pathPrefix + "Projectile.png")) throw std::runtime_error("Failed to load Projectile.png");

    std::cout << "Textures loaded successfully." << std::endl;
}

void Game::loadSounds() {
    std::string pathPrefix = "sprites/";

    // --- background music ---
    if (!mBackgroundMusic.openFromFile(pathPrefix + "GameMusic.wav")) {
        std::cerr << "Warning: Failed to load GameMusic.wav\n";
    } else {
        std::cout << "Background music loaded successfully.\n";
    }

    // --- tower shoot sound ---
    if (!mTowerShotBuf.loadFromFile(pathPrefix + "tower_shot.wav")) {
        std::cerr << "Failed to load tower_shot.wav\n";
    }
    mTowerShotSound.setBuffer(mTowerShotBuf);

    // --- tower damage sounds (alternates) ---
    if (!mTowerDmgBuf1.loadFromFile(pathPrefix + "dmg_1.wav")) {
        std::cerr << "Failed to load dmg_1.wav\n";
    }
    mTowerDmgSound1.setBuffer(mTowerDmgBuf1);

    if (!mTowerDmgBuf2.loadFromFile(pathPrefix + "dmg_2.wav")) {
        std::cerr << "Failed to load dmg_2.wav\n";
    }
    mTowerDmgSound2.setBuffer(mTowerDmgBuf2);
}


// --- initial setup ---
void Game::setupInitialState() {
    // window size
    int windowWidth = mWindow.getSize().x;
    int windowHeight = mWindow.getSize().y;

    // ------------------ random object placement (rocks, flowers, etc...) --------------
    // random number generator to randomly place the objects around the map
    std::random_device rd;
    std::mt19937 gen(rd());.
    std::uniform_int_distribution<> distX(0, windowWidth);
    std::uniform_int_distribution<> distY(0, windowHeight);

    // save positions of already placed objects
    std::vector<sf::FloatRect> placedBounds;

    // randomly place 20 rocks
    const int numRocks = 20;
    const int maxAttemptsPerObject = 100; // to avoid infinite loop
    for (int i = 0; i < numRocks; i++) {
        bool placed = false;
        int attempts = 0;
        while (!placed && attempts < maxAttemptsPerObject) {
            // generate a position
            int x = distX(gen);
            int y = distY(gen);

            // create sprite
            sf::Sprite rock(mRockTexture);
            rock.setPosition(static_cast<float>(x), static_cast<float>(y));

            // get its bounds
            sf::FloatRect bounds = rock.getGlobalBounds();

            // adjust position to stay on the screen
            if (bounds.left + bounds.width > windowWidth)
                rock.setPosition(windowWidth - bounds.width, rock.getPosition().y);
            if (bounds.top + bounds.height > windowHeight)
                rock.setPosition(rock.getPosition().x, windowHeight - bounds.height);

            //recalculate bounds
            bounds = rock.getGlobalBounds();

            // is it in the window?
            if (bounds.left < 0 || bounds.top < 0 || 
                bounds.left + bounds.width > windowWidth || 
                bounds.top + bounds.height > windowHeight) {
                attempts++;
                continue;
            }

            // is there any overlap?
            bool overlaps = false;
            for (const auto& placedRect : placedBounds) {
                if (placedRect.intersects(bounds)) {
                    overlaps = true;
                    break;
                }
            }

            if (!overlaps) {
                // accept placement
                placed = true;
                placedBounds.push_back(bounds);
                mStaticObjects.push_back(rock);
            }

            attempts++;
        }

        // if error... output the specific error
        if (!placed) {
            std::cerr << "Failed to place rock #" << i << " without overlap after " << maxAttemptsPerObject << " attempts." << std::endl;
        }
    }
    
    // create 5 random trees on the map
    const int numTrees = 5;
    for (int i = 0; i < numTrees; i++) {
        bool placed = false;
        int attempts = 0;
        while (!placed && attempts < maxAttemptsPerObject) {
            int x = distX(gen);
            int y = distY(gen);

            sf::Sprite tree(mTreeTexture);
            tree.setPosition(static_cast<float>(x), static_cast<float>(y));

            sf::FloatRect bounds = tree.getGlobalBounds();
            if (bounds.left + bounds.width > windowWidth)
                tree.setPosition(windowWidth - bounds.width, tree.getPosition().y);
            if (bounds.top + bounds.height > windowHeight)
                tree.setPosition(tree.getPosition().x, windowHeight - bounds.height);
            bounds = tree.getGlobalBounds();

            if (bounds.left < 0 || bounds.top < 0 ||
                bounds.left + bounds.width > windowWidth ||
                bounds.top + bounds.height > windowHeight) {
                attempts++;
                continue;
            }

            bool overlaps = false;
            for (const auto& placedRect : placedBounds) {
                if (placedRect.intersects(bounds)) {
                    overlaps = true;
                    break;
                }
            }

            if (!overlaps) {
                placed = true;
                placedBounds.push_back(bounds);
                mStaticObjects.push_back(tree);
            }

            attempts++;
        }
        if (!placed)
            std::cerr << "Failed to place tree #" << i 
                    << " without overlap after " << maxAttemptsPerObject << " attempts." 
                    << std::endl;
    }


    // create random flowers, we have 3 different textures
    std::vector<sf::Texture*> flowerTextures = {
        &mFlowerFirstTexture,
        &mFlowerSecondTexture,
        &mFlowerThirdTexture
    };

    const int numFlowerTypes = static_cast<int>(flowerTextures.size());
    // create 7 of each flower
    const int flowersPerType = 7;
    
    for (int type = 0; type < numFlowerTypes; type++) {
        for (int i = 0; i < flowersPerType; i++) {
            bool placed = false;
            int attempts = 0;
            while (!placed && attempts < maxAttemptsPerObject) {
                int x = distX(gen);
                int y = distY(gen);
    
                sf::Sprite flower(*flowerTextures[type]);
                flower.setPosition(static_cast<float>(x), static_cast<float>(y));
                sf::FloatRect bounds = flower.getGlobalBounds();
    
                if (bounds.left + bounds.width > windowWidth)
                    flower.setPosition(windowWidth - bounds.width, flower.getPosition().y);
                if (bounds.top + bounds.height > windowHeight)
                    flower.setPosition(flower.getPosition().x, windowHeight - bounds.height);
                bounds = flower.getGlobalBounds();
    
                if (bounds.left < 0 || bounds.top < 0 ||
                    bounds.left + bounds.width > windowWidth ||
                    bounds.top + bounds.height > windowHeight) {
                    attempts++;
                    continue;
                }
    
                bool overlaps = false;
                for (const auto& placedRect : placedBounds) {
                    if (placedRect.intersects(bounds)) {
                        overlaps = true;
                        break;
                    }
                }
    
                if (!overlaps) {
                    placed = true;
                    placedBounds.push_back(bounds);
                    mStaticObjects.push_back(flower);
                }
                attempts++;
            }
            if (!placed) {
                std::cerr << "Failed to place flower of type " << type + 1 
                          << " instance " << i 
                          << " without overlap after " << maxAttemptsPerObject << " attempts." 
                          << std::endl;
            }
        }
    }

    std::cout << "Initial state setup complete." << std::endl;
}
// ------------- END random object placement ---------------



// ------------- setup moving animations (ie: birds, tumbleweeds) -------------
void Game::setupAnimations() {
    // tumbleweeds
    mTumbleweedSprite.setTexture(mTumbleweedTexture);
    mTumbleweedSprite2.setTexture(mTumbleweedTexture);
    mTumbleweedSprite.setTextureRect(sf::IntRect(0, 0, 100, 100));
    mTumbleweedSprite2.setTextureRect(sf::IntRect(0, 0, 100, 100));
    mTumbleweedSprite2.setScale(-1.0f, 1.0f);
    mTumbleweedPosition = sf::Vector2f(1920, 800); // for starting from the right side
    mTumbleweedPosition2 = sf::Vector2f(-100, 200); // for starting from the left side
    mTumbleweedSprite.setPosition(mTumbleweedPosition);
    mTumbleweedSprite2.setPosition(mTumbleweedPosition2);

    // birds
    mBirdSprite.setTexture(mBirdTexture);
    mBirdSprite2.setTexture(mBirdTexture);
    mBirdSprite.setTextureRect(sf::IntRect(0, 0, 135, 92));
    mBirdSprite2.setTextureRect(sf::IntRect(0, 0, 135, 92));
    mBirdSprite2.setScale(-1.0f, 1.0f);
    sf::FloatRect birdBounds = mBirdSprite.getLocalBounds();
    mBirdSprite.setOrigin(birdBounds.width / 2.f, birdBounds.height / 2.f);
    mBirdSprite2.setOrigin(birdBounds.width / 2.f, birdBounds.height / 2.f);
    mBirdPosition = sf::Vector2f(1920 + birdBounds.width, 50);
    mBirdPosition2 = sf::Vector2f(0 - birdBounds.width, 700);
    mBirdSprite.setPosition(mBirdPosition);
    mBirdSprite2.setPosition(mBirdPosition2);

    std::cout << "Animations setup complete." << std::endl;
}
//-------------- END moving animation setup -------------------



// --- main Loop ---
void Game::run() {
    sf::Clock clock;
    int   frameCount      = 0;
    float framePrintTimer = 0.0f;
    const float printInterval = 2.0f;  // log interval.. debug

    mGameTime = 0.0f;

    while (mWindow.isOpen()) {
        // time
        sf::Time deltaTime = clock.restart();
        float dtSeconds = deltaTime.asSeconds();

        // advance time
        mGameTime += dtSeconds;

       // debug for each frame
        framePrintTimer += dtSeconds;
        if (framePrintTimer >= printInterval) {
            std::cout 
                << "[" 
                << std::fixed << std::setprecision(2)
                << mGameTime << "s] "
                << "Frame " << frameCount 
                << ": dt = " << dtSeconds << " sec\n";
            framePrintTimer = 0.0f;
        }
        frameCount++;

        // process one frame
        processEvents();
        update(deltaTime);
        render();
    }
}



// --- event processing ---
void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        } else if (event.type == sf::Event::MouseButtonPressed) {

            // place a tower on the left mouse click
            if (!mIsGameOver && event.mouseButton.button == sf::Mouse::Left && mTowers.size() < mMaxTowers) {
                sf::Vector2f mousePos = mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                Tower newTower(mTowerTexture);
                newTower.shape.setPosition(mousePos);
                mTowers.push_back(newTower);
            }
        }
    }
}

// update the game
void Game::update(sf::Time deltaTime) {
    float dtSeconds = deltaTime.asSeconds();

    // if the game is over.. exit
    if (mIsGameOver)
        return;

    // spawn enemies
    mWaveManager.update(dtSeconds,
                        mEnemies,
                        mNextEnemyIndex,
                        mEnemyTexture,
                        mPathManager,
                        mGameTime);

    // enemies will move and attack
    for (auto& enemy : mEnemies) {
        if (enemy.isDead)
            continue;

        // is a tower in range?
        Tower* targetTower = nullptr;
        for (auto& tower : mTowers) {
            if (tower.isAlive() &&
                tower.isInRange(enemy.body.getPosition()))
            {
                targetTower = &tower;
                break;
            }
        }

        // if the enemy is hit, attack the tower
        if (!targetTower && enemy.aggroed) {
            float bestDist2 = std::numeric_limits<float>::max();
            for (auto& tower : mTowers) {
                if (!tower.isAlive()) continue;
                sf::Vector2f d = tower.shape.getPosition()
                              - enemy.body.getPosition();
                float dist2 = d.x*d.x + d.y*d.y;
                if (dist2 < bestDist2) {
                    bestDist2   = dist2;
                    targetTower = &tower;
                }
            }
        }

        // if we target a tower, move into range
        if (targetTower) {
            enemy.isAttackingTower = true;
            enemy.targetTower      = targetTower;
            
            sf::Vector2f diff = targetTower->shape.getPosition()
                              - enemy.body.getPosition();
            float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);

            const float MELEE_RANGE = 30.f;  // range
            // do we need to move into range?
            if (dist > MELEE_RANGE) {
                diff /= dist;  
                enemy.body.move(diff * enemy.movementSpeed * dtSeconds);
            } else {
                // if not, attack
                enemy.updateAttackTimer(dtSeconds);
                // play damage sounds
                if (enemy.tryAttackTower()) {
                    if (mToggleDmgSound) mTowerDmgSound1.play();
                    else                mTowerDmgSound2.play();
                    mToggleDmgSound = !mToggleDmgSound;
                }
            }
        }
        else {
            // resume path to base
            enemy.isAttackingTower = false;
            enemy.targetTower      = nullptr;

            // or we are already attacking it, so update
            if (enemy.isAttacking) {
                enemy.updateAttackTimer(dtSeconds);
                if (enemy.tryAttackBase()) {
                    // play sound... needs to be added
                }
            } else {
                // move along the path
                enemy.updateMovement(dtSeconds,
                                     mPathManager,
                                     mEnemies);
            }
        }
    }

    // towers will attack enemy
    for (auto& tower : mTowers) {
        for (auto& enemy : mEnemies) {
            if (!enemy.isDead && tower.attackEnemy(enemy, dtSeconds)) {
                mTowerShotSound.play();
                Projectile p(mProjectileTexture, tower.damage, mProjectileSpeed);
                p.launch(tower.shape.getPosition(), enemy.body.getPosition());
                mProjectiles.push_back(std::move(p));
                break;
            }
        }
    }

    // projectile that hits the enemy
    for (auto& proj : mProjectiles) {
        if (!proj.isActive()) continue;
        proj.update(dtSeconds);
        for (auto& enemy : mEnemies) {
            if (!enemy.isDead &&
                proj.getSprite().getGlobalBounds().intersects(enemy.body.getGlobalBounds()))
            {
                enemy.takeDamage(proj.getDamage());
                proj.deactivate();
                break;
            }
        }
    }

    mProjectiles.erase(
        std::remove_if(mProjectiles.begin(), mProjectiles.end(),
            [](const Projectile& p){ return !p.isActive(); }),
        mProjectiles.end());

    // if a tower dies, erase it
    mTowers.erase(
        std::remove_if(mTowers.begin(), mTowers.end(),
            [](const Tower& t){ return !t.isAlive(); }),
        mTowers.end());
    

    // ------------ update animations -------------
    // tumbleweeds
    mElapsedTime += dtSeconds;
    if (mElapsedTime >= frameSwitchTime) {
        mFrameIndex = (mFrameIndex + 1) % 4; // 4 frames
        sf::IntRect textureRect(mFrameIndex * 100, 0, 100, 100);
        mTumbleweedSprite.setTextureRect(textureRect);
        mTumbleweedSprite2.setTextureRect(textureRect);
        mElapsedTime = 0.0f; // reset frame timer
    }

    // move the right tumbleweed
    mTumbleweedPosition.x += mTumbleweedSpeed * dtSeconds;
    if (mTumbleweedPosition.x < -100) { // check if its to the left
        mTumbleweedPosition.x = mWindow.getSize().x + 100; // reset it to the right
    }
    mTumbleweedSprite.setPosition(mTumbleweedPosition);

    // move left tumble weed
    mTumbleweedPosition2.x += mTumbleweedSpeed2 * dtSeconds;
    if (mTumbleweedPosition2.x > mWindow.getSize().x + 100) {
        mTumbleweedPosition2.x = -100;
    }
    mTumbleweedSprite2.setPosition(mTumbleweedPosition2);

    // birds
    mBirdAnimationTime += dtSeconds;
    if (mBirdAnimationTime >= birdFrameSwitchTime) {
         mBirdFrameIndex = (mBirdFrameIndex + 1) % 2; // 2 frames
         sf::IntRect textureRect(mBirdFrameIndex * 135, 0, 135, 92);
         mBirdSprite.setTextureRect(textureRect);
         mBirdSprite2.setTextureRect(textureRect);
         mBirdAnimationTime = 0.0f;
    }

    mBirdPosition.x += mBirdSpeed * dtSeconds;
    if (mBirdPosition.x < 0 - mBirdSprite.getGlobalBounds().width) {
         mBirdPosition.x = mWindow.getSize().x + mBirdSprite.getGlobalBounds().width;
    }
    mBirdSprite.setPosition(mBirdPosition);
    mBirdPosition2.x += mBirdSpeed2 * dtSeconds;
    if (mBirdPosition2.x > mWindow.getSize().x + mBirdSprite2.getGlobalBounds().width) {
         mBirdPosition2.x = 0 - mBirdSprite2.getGlobalBounds().width;
    }
    mBirdSprite2.setPosition(mBirdPosition2);
}
// ------------ end update animations -------------

// ------- game rendering -------
void Game::render() {
    mWindow.clear(sf::Color(180, 220, 255));
    mWindow.draw(mMapSprite);

    //draw static objects (rocks, flowers, etc)
    for (const auto& object : mStaticObjects) {
        mWindow.draw(object);
    }

    // draw tumbleweeds
    mWindow.draw(mTumbleweedSprite);
    mWindow.draw(mTumbleweedSprite2);

    // draw player base
    if (mPlayerBase) {
        mPlayerBase->draw(mWindow);
    }

    // draw towers
    for (auto& tower : mTowers) {
        tower.draw(mWindow);
    }

    // draw enemies
    for (auto& enemy : mEnemies) {
         enemy.draw(mWindow);
    }

    // draw projectiles
    for (const auto & proj : mProjectiles) {
        if (proj.isActive())
            mWindow.draw(proj.getSprite());
    }

    // draw birds
    mWindow.draw(mBirdSprite);
    mWindow.draw(mBirdSprite2);

    if (mIsGameOver) {
        sf::Font font;
        // not implemented yet...
    }

    mWindow.display();
}