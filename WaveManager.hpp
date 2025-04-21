#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Texture.hpp>

class Enemy;
class PathManager;

class WaveManager {
public:
    struct Wave {
        int   count;            // enemies in the wave
        float initialDelay;     // delay before wave
        float spawnInterval;    // time between enemy spawns
        float enemySpeed;       // speed of wave
        float spawnAccumulator;

        Wave(int c, float d, float s, float v)
        : count(c), initialDelay(d), spawnInterval(s), enemySpeed(v), spawnAccumulator(0.0f)
        {}
    };

    WaveManager();

    bool areAllWavesComplete() const;

    void update(float deltaTime, 
                std::vector<Enemy>& enemies,
                int& nextAvailableEnemyIndex,
                sf::Texture& enemyTexture,
                PathManager& pathManager,
                float gameTime);
private:
    void startNextWave();
    void startNextWave(float gameTime);

    float calculateSpeed(size_t waveIndex) const;

    std::vector<Wave> waves;
    size_t            currentWaveIndex;
    float             waveTimer;
    int               enemiesSpawnedThisWave;
    bool              waitingForNextWave;
};
