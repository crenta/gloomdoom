#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <stdexcept>

#include "Enemy.hpp"
#include "Tower.hpp"
#include "PlayerBase.hpp"
#include "WaveManager.hpp"
#include "PathManager.hpp"
#include "Projectile.hpp"


class Game {
public:
    Game();

    ~Game();

    void run();

private:

    // --- core methods ---
    void processEvents();       // handles user tower placement and window events
    void update(sf::Time deltaTime); // update the game state
    void render();              // draw to the window

    // --- setup methods ---
    void loadTextures();        // load all the textures
    void loadSounds();          // load sound and effects
    void setupInitialState();   // Sets up map, objects, and enemy pools
    void setupAnimations();     // set up the animated events

    // --- window & audio ---
    sf::RenderWindow mWindow;          // main game window
    sf::Music mBackgroundMusic;        // background music

    // sound buffers
    sf::SoundBuffer mTowerShotBuf;
    sf::SoundBuffer mTowerDmgBuf1;
    sf::SoundBuffer mTowerDmgBuf2;


    // sounds
    sf::Sound mTowerShotSound;
    sf::Sound mTowerDmgSound1;
    sf::Sound mTowerDmgSound2;
    
    float mGameTime = 0.0f;

    // used to alternate the damage sound
    bool mToggleDmgSound = false;

    // --- textures ---
    sf::Texture mMapTexture;
    sf::Texture mEnemyTexture;
    sf::Texture mTowerTexture;
    sf::Texture mBaseTexture;
    sf::Texture mTumbleweedTexture;
    sf::Texture mBirdTexture;
    sf::Texture mRockTexture;
    sf::Texture mTreeTexture;
    sf::Texture mFlowerFirstTexture;
    sf::Texture mFlowerSecondTexture;
    sf::Texture mFlowerThirdTexture;

    // --- objects & managers ---
    sf::Sprite mMapSprite;             // background map
    std::vector<sf::Sprite> mStaticObjects; // objects

    PlayerBase* mPlayerBase;         // main base
    PathManager mPathManager;        // enemy path manager
    WaveManager mWaveManager;        // enemy wave manager

    std::vector<Enemy> mEnemies;       // the pool of enemies
    std::vector<Tower> mTowers;        // towers placed by the player

    std::vector<Projectile> mProjectiles;
    sf::Texture mProjectileTexture;   // projectile texture
    float mProjectileSpeed = 300.f;   // projectile speed

    int mNextEnemyIndex;             // next enemy index
    const size_t mMaxTowers = 13;    // max number of towers

    // ---- animations ----
    sf::Sprite mTumbleweedSprite, mTumbleweedSprite2;
    sf::Sprite mBirdSprite, mBirdSprite2;
    sf::Vector2f mTumbleweedPosition, mTumbleweedPosition2;
    sf::Vector2f mBirdPosition, mBirdPosition2;
    float mTumbleweedSpeed = -200.0f, mTumbleweedSpeed2 = 200.0f;
    float mBirdSpeed = -200.0f, mBirdSpeed2 = 200.0f;
    float mElapsedTime = 0.0f, mBirdAnimationTime = 0.0f;
    int mFrameIndex = 0, mBirdFrameIndex = 0;
    const float frameSwitchTime = 0.2f; // time between tumbleweeds
    const float birdFrameSwitchTime = 0.1f; // time between birds

    // game state
    bool mIsGameOver = false;        // game over?
};