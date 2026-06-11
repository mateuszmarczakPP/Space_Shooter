#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // <- NOWE: Wymagane dla dźwięków gracza
#include <cmath>
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

    // --- NOWE: AUDIO GRACZA ---
    sf::SoundBuffer shootBuffer;
    sf::Sound shootSound;
    sf::SoundBuffer thrustBuffer;
    sf::Sound thrustSound;
    sf::SoundBuffer reloadBuffer;
    sf::Sound reloadSound;

    void initTexture();
    void initSprite();
    void initAudio(); // <- NOWE: Inicjalizacja audio

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

    // GETTERY I METODY DLA AMUNICJI
    int getAmmo() const;
    int getAmmoMax() const;
    bool isReloading() const;
    void startReload();
    void stopSounds(); // <- NOWE: Zatrzymanie zapętlonych dźwięków
};

#endif