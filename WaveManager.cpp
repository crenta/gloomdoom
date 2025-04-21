#include "WaveManager.hpp"
#include "Enemy.hpp"
#include "PathManager.hpp"
#include <iostream>
#include <iomanip>

WaveManager::WaveManager()
: currentWaveIndex(0)
, waveTimer(0.0f)
, enemiesSpawnedThisWave(0)
, waitingForNextWave(true)
{
    // waves definitions... (count #, initialDelay(delay time), spawnInterval(time between spawn), speed)
    waves.emplace_back(3, 2.0f, 3.5f, calculateSpeed(0));
    waves.emplace_back(5, 3.0f, 3.2f, calculateSpeed(1));
    waves.emplace_back(8, 4.0f, 3.0f, calculateSpeed(2));
    // add more waves in the future

    if (!waves.empty())
        waveTimer = waves[0].initialDelay;
    else
        waitingForNextWave = false;
}

bool WaveManager::areAllWavesComplete() const {
    return currentWaveIndex >= waves.size();
}

// start the next wave and log game time
void WaveManager::startNextWave(float gameTime) {
    enemiesSpawnedThisWave = 0;
    waves[currentWaveIndex].spawnAccumulator = 0.0f;
    waitingForNextWave = false;

    std::cout 
        << "[" << std::fixed << std::setprecision(2)
        << gameTime << "s] "
        << "Wave " << (currentWaveIndex + 1) << " started."
        << std::endl;
}

// calculate the speed
float WaveManager::calculateSpeed(size_t waveIndex) const {
    return 100.f + 10.f * static_cast<float>(waveIndex);
}

// update the wave
void WaveManager::update(
    float deltaTime,
    std::vector<Enemy>& enemies,
    int&,
    sf::Texture&,
    PathManager& pathManager,
    float gameTime)
{
    // if all waves are done... complete message
    if (areAllWavesComplete()) {
        static bool printed = false;
        if (!printed) {
            std::cout << "[" << std::fixed << std::setprecision(2)
                      << gameTime << "s] All waves complete."
                      << std::endl;
            printed = true;
        }
        return;
    }

    // if we are waiting for another wave...
    if (waitingForNextWave) {
        //make sure the previous wave is cleared
        bool anyAlive = false;
        for (auto& e : enemies) {
            if (!e.isDead) {
                anyAlive = true;
                break;
            }
        }
        if (anyAlive) {
            // if enemies are on the field wait
            return;
        }

        // the field is clear... decrement time
        waveTimer -= deltaTime;
        // start the next wave
        if (waveTimer <= 0.0f) {
            startNextWave(gameTime);
        }
        return;
    }

    // spawn the wave
    Wave& wave = waves[currentWaveIndex];
    wave.spawnAccumulator += deltaTime;

    // one spawn per frame
    if (wave.spawnAccumulator >= wave.spawnInterval
        && enemiesSpawnedThisWave < wave.count)
    {
        bool spawned = false;
        for (auto& e : enemies) {
            if (e.isDead) {
                e.activate(pathManager.getStartPoint(), wave.enemySpeed);
                ++enemiesSpawnedThisWave;

                std::cout 
                    << "[" << std::fixed << std::setprecision(2)
                    << gameTime << "s] "
                    << "Spawned enemy " << enemiesSpawnedThisWave
                    << " of " << wave.count
                    << " for Wave " << (currentWaveIndex + 1)
                    << std::endl;

                spawned = true;
                break;
            }
        }

        if (!spawned) {
            std::cerr << "[" << std::fixed << std::setprecision(2)
                      << gameTime << "s] "
                      << "Warning: Enemy pool depleted!"
                      << std::endl;
            enemiesSpawnedThisWave = wave.count;
        }

        wave.spawnAccumulator -= wave.spawnInterval;
    }

    // if the wave is done prepare for the next wave
    if (enemiesSpawnedThisWave >= wave.count) {
        std::cout 
            << "[" << std::fixed << std::setprecision(2)
            << gameTime << "s] "
            << "Wave " << (currentWaveIndex + 1)
            << " spawning complete."
            << std::endl;

    

        ++currentWaveIndex;
        if (!areAllWavesComplete()) {
            waitingForNextWave = true;
            waveTimer = waves[currentWaveIndex].initialDelay;
            std::cout 
                << "[" << std::fixed << std::setprecision(2)
                << gameTime << "s] "
                << "Next wave in " << waveTimer << " sec."
                << std::endl;
        }
    }

    // move enemies along the path
    for (auto& e : enemies) {
    if (!e.isDead) {
        e.updateMovement(deltaTime, pathManager, enemies);
        }
    }
}

