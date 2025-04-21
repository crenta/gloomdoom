#include "Projectile.hpp"

Projectile::Projectile(const sf::Texture& texture, float damage, float speed)
    : mSpeed(speed), mDamage(damage), mActive(false)
{
    // initialize the projectile
    mSprite.setTexture(texture);
    sf::FloatRect bounds = mSprite.getLocalBounds();
    mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    mSprite.setScale(0.3f, 0.3f);
}

// launch the projectile
void Projectile::launch(const sf::Vector2f& start, const sf::Vector2f& target) {
    mSprite.setPosition(start);
    sf::Vector2f diff = target - start;
    float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (length != 0)
        mDirection = diff / length;
    else
        mDirection = sf::Vector2f(0.f, 0.f);

    mActive = true;
    float angle = std::atan2(mDirection.y, mDirection.x) * 180.f / 3.14159265f;
    mSprite.setRotation(angle);
}

// update the position
void Projectile::update(float deltaTime) {
    if (!mActive)
        return;
    mSprite.move(mDirection * mSpeed * deltaTime);
}

const sf::Sprite& Projectile::getSprite() const {
    return mSprite;
}

bool Projectile::isActive() const {
    return mActive;
}

float Projectile::getDamage() const {
    return mDamage;
}

void Projectile::deactivate() {
    mActive = false;
}
