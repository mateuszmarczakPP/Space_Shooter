#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include "Bullet.h"

class Player {
private:
    sf::Sprite sprite;
    sf::Texture texture;

    sf::Vector2f velocity;
    void loadConfig();
    float acceleration;
    float drag;
    float maxVelocity;
    float rotationSpeed;

    int shootTimer;
    int shootTimerMax;

    void initTexture();
    void initSprite();

public:
    Player();
    virtual ~Player();

    void update(const sf::RenderTarget* target);
    void render(sf::RenderTarget& target);

    bool canShoot();
    Bullet* shoot(sf::Texture* bulletTexture);

    const sf::Vector2f& getPos() const;
    float getRotation() const;
    const sf::FloatRect getBounds() const {
        sf::FloatRect bounds = this->sprite.getGlobalBounds();

        float margin = 20.f;

        return sf::FloatRect(
            bounds.left + margin,
            bounds.top + margin,
            bounds.width - (2.f * margin),
            bounds.height - (2.f * margin)
        );
    }
    void resetPosition();
};

#endif