#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector> // <- NOWE: Wymagane do przechowywania cząsteczek
#include "Bullet.h"

class Player {
private:
    sf::Sprite sprite;
    sf::Texture textureIdle;
    sf::Texture textureThrust;

    sf::Vector2f velocity;
    void loadConfig();
    float acceleration;
    float drag;
    float maxVelocity;
    float rotationSpeed;

    int shootTimer;
    int shootTimerMax;

    // MECHANIKA AMUNICJI
    int ammo;
    int ammoMax;
    bool reloading;
    int reloadTimer;
    int reloadTimerMax;

    // AUDIO GRACZA
    sf::SoundBuffer shootBuffer;
    sf::Sound shootSound;
    sf::SoundBuffer thrustBuffer;
    sf::Sound thrustSound;
    sf::SoundBuffer reloadBuffer;
    sf::Sound reloadSound;

    // --- NOWE: SYSTEM CZĄSTECZEK (DYM Z SILNIKA) ---
    struct ExhaustParticle {
        sf::RectangleShape shape;
        sf::Vector2f velocity;
        int lifetime;
        int maxLifetime;
    };
    std::vector<ExhaustParticle> exhaustParticles;

    void initTexture();
    void initSprite();
    void initAudio();

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

    int getAmmo() const;
    int getAmmoMax() const;
    bool isReloading() const;
    void startReload();
    void stopSounds();
};

#endif