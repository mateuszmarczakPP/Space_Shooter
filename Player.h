#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include "GameObject.h"
#include "Bullet.h"

class Player : public GameObject {
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
    int ammo;
    int ammoMax;
    bool reloading;
    int reloadTimer;
    int reloadTimerMax;

    sf::SoundBuffer shootBuffer, thrustBuffer, reloadBuffer;
    sf::Sound shootSound, thrustSound, reloadSound;

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

    void update(sf::RenderTarget* target) override;
    void render(sf::RenderTarget* target) override;

    const sf::FloatRect getBounds() const override {
        sf::FloatRect bounds = this->sprite.getGlobalBounds();
        float margin = 20.f;
        return sf::FloatRect(bounds.left + margin, bounds.top + margin, bounds.width - (2.f * margin), bounds.height - (2.f * margin));
    }

    bool canShoot();
    Bullet* shoot(sf::Texture* bulletTexture);
    const sf::Vector2f& getPos() const;
    float getRotation() const;
    void resetPosition();

    // NOWE: Funkcja do wczytywania gry
    void loadState(int loadedAmmo, float px, float py);

    int getAmmo() const;
    int getAmmoMax() const;
    bool isReloading() const;
    void startReload();
    void stopSounds();
};

#endif