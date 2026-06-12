#include "Player.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // Wymagane dla funkcji rand()

Player::Player() {
    this->loadConfig();
    this->velocity = sf::Vector2f(0.f, 0.f);
    this->shootTimer = this->shootTimerMax;

    // Inicjalizacja amunicji
    this->ammoMax = 20;
    this->ammo = this->ammoMax;
    this->reloading = false;
    this->reloadTimer = 0;
    this->reloadTimerMax = 240;

    this->initTexture();
    this->initSprite();
    this->initAudio();
}

Player::~Player() {}

void Player::initAudio() {
    if (!this->shootBuffer.loadFromFile("shoot.wav"))
        std::cout << "ERROR::PLAYER::Nie mozna wczytac shoot.wav" << std::endl;
    this->shootSound.setBuffer(this->shootBuffer);
    this->shootSound.setVolume(50.f);

    if (!this->thrustBuffer.loadFromFile("thrust.wav"))
        std::cout << "ERROR::PLAYER::Nie mozna wczytac thrust.wav" << std::endl;
    this->thrustSound.setBuffer(this->thrustBuffer);
    this->thrustSound.setLoop(true);
    this->thrustSound.setVolume(50.f);

    if (!this->reloadBuffer.loadFromFile("reload.wav"))
        std::cout << "ERROR::PLAYER::Nie mozna wczytac reload.wav" << std::endl;
    this->reloadSound.setBuffer(this->reloadBuffer);
    this->reloadSound.setVolume(50.f);
}

void Player::loadConfig() {
    std::ifstream configFile("config.txt");
    this->acceleration = 0.6f;
    this->drag = 0.9f;
    this->maxVelocity = 10.f;
    this->rotationSpeed = 4.f;
    this->shootTimerMax = 30;

    if (configFile.is_open()) {
        std::string key;
        float value;
        while (configFile >> key >> value) {
            if (key == "acceleration")        this->acceleration = value;
            else if (key == "drag")           this->drag = value;
            else if (key == "maxVelocity")    this->maxVelocity = value;
            else if (key == "rotationSpeed")  this->rotationSpeed = value;
            else if (key == "shootTimerMax")  this->shootTimerMax = static_cast<int>(value);
        }
        configFile.close();
        std::cout << "Konfiguracja wczytana pomyslnie!\n";
    } else {
        std::cout << "Blad: Nie znaleziono config.txt. Uzywam ustawien domyslnych.\n";
    }
}

void Player::initTexture() {
    if (!this->textureIdle.loadFromFile("ship.png"))
        std::cout << "ERROR::PLAYER::Could not load texture ship.png!" << std::endl;
    if (!this->textureThrust.loadFromFile("ship_thrust.png"))
        std::cout << "ERROR::PLAYER::Could not load texture ship_thrust.png!" << std::endl;
}

void Player::initSprite() {
    this->sprite.setTexture(this->textureIdle);
    this->sprite.setScale(0.3f, 0.3f);
    sf::FloatRect bounds = this->sprite.getLocalBounds();
    this->sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    this->sprite.setPosition(640.f, 360.f);
}

bool Player::canShoot() {
    if (this->reloading) return false;

    if (this->ammo <= 0) {
        this->startReload();
        return false;
    }

    if (this->shootTimer >= this->shootTimerMax) {
        this->shootTimer = 0;
        this->ammo--;
        this->shootSound.play();
        return true;
    }
    return false;
}

Bullet* Player::shoot(sf::Texture* bulletTexture) {
    float angle = (this->sprite.getRotation() - 90.f) * 3.14159265f / 180.f;
    return new Bullet(bulletTexture, this->sprite.getPosition().x, this->sprite.getPosition().y,
                      std::cos(angle), std::sin(angle), 15.f);
}

void Player::update(const sf::RenderTarget* target) {
    if (this->reloading) {
        this->reloadTimer++;
        if (this->reloadTimer >= this->reloadTimerMax) {
            this->ammo = this->ammoMax;
            this->reloading = false;
            this->reloadTimer = 0;
        }
    } else {
        if (this->shootTimer < this->shootTimerMax) this->shootTimer++;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && this->ammo < this->ammoMax) {
            this->startReload();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) this->sprite.rotate(-this->rotationSpeed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) this->sprite.rotate(this->rotationSpeed);

    // --- RUCH I EFEKTY SILNIKA ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        this->sprite.setTexture(this->textureThrust);

        if (this->thrustSound.getStatus() != sf::Sound::Playing) {
            this->thrustSound.play();
        }

        float angle = (this->sprite.getRotation() - 90.f) * 3.14159265f / 180.f;
        this->velocity.x += std::cos(angle) * this->acceleration;
        this->velocity.y += std::sin(angle) * this->acceleration;

        // --- TWORZENIE CZĄSTECZEK DYMU Z SILNIKA ---
        // Generujemy 2 cząsteczki na klatkę dla gęstszego dymu
        for (int i = 0; i < 2; i++) {
            ExhaustParticle p;
            p.shape.setSize(sf::Vector2f(8.f, 8.f));
            p.shape.setOrigin(4.f, 4.f);

            // Obliczamy tył statku (przeciwność kierunku lotu + przesunięcie)
            float backX = this->sprite.getPosition().x - std::cos(angle) * 25.f;
            float backY = this->sprite.getPosition().y - std::sin(angle) * 25.f;

            // Dodajemy lekki rozrzut (od -4 do +4 pikseli)
            backX += (rand() % 9 - 4);
            backY += (rand() % 9 - 4);
            p.shape.setPosition(backX, backY);

            // Prędkość cząsteczki (lekko dryfuje do tyłu)
            p.velocity.x = -std::cos(angle) * (1.f + (rand() % 10) * 0.1f);
            p.velocity.y = -std::sin(angle) * (1.f + (rand() % 10) * 0.1f);

            p.maxLifetime = 15 + rand() % 15; // Cząsteczka żyje krótko (ok. pół sekundy)
            p.lifetime = p.maxLifetime;

            // Kolor startowy (Losowy odcień ognia: pomarańczowy/żółty)
            p.shape.setFillColor(sf::Color(255, 150 + rand() % 105, 0, 255));

            this->exhaustParticles.push_back(p);
        }

    } else {
        this->sprite.setTexture(this->textureIdle);

        if (this->thrustSound.getStatus() == sf::Sound::Playing) {
            this->thrustSound.stop();
        }
    }

    // --- AKTUALIZACJA CZĄSTECZEK DYMU ---
    for (int i = 0; i < this->exhaustParticles.size(); i++) {
        this->exhaustParticles[i].shape.move(this->exhaustParticles[i].velocity);
        this->exhaustParticles[i].lifetime--;

        float progress = static_cast<float>(this->exhaustParticles[i].lifetime) / this->exhaustParticles[i].maxLifetime;

        // Zmniejszanie się cząsteczki pod koniec życia
        this->exhaustParticles[i].shape.setScale(progress, progress);

        // Magia zmiany koloru: z ognistego na szary dym + zanikanie (przezroczystość)
        sf::Color c = this->exhaustParticles[i].shape.getFillColor();
        c.a = static_cast<sf::Uint8>(255 * progress);
        c.r = static_cast<sf::Uint8>(255 * progress + 80 * (1.0f - progress)); // 80 to odcień szarego
        c.g = static_cast<sf::Uint8>(150 * progress + 80 * (1.0f - progress));
        c.b = static_cast<sf::Uint8>(0 * progress + 80 * (1.0f - progress));

        this->exhaustParticles[i].shape.setFillColor(c);

        // Usuwanie martwych cząsteczek
        if (this->exhaustParticles[i].lifetime <= 0) {
            this->exhaustParticles.erase(this->exhaustParticles.begin() + i);
            i--;
        }
    }

    float speed = std::sqrt(this->velocity.x * this->velocity.x + this->velocity.y * this->velocity.y);
    if (speed > this->maxVelocity) this->velocity = (this->velocity / speed) * this->maxVelocity;
    this->velocity *= this->drag;
    this->sprite.move(this->velocity);

    // BLOKADA WYJŚCIA POZA EKRAN
    sf::FloatRect bounds = this->sprite.getGlobalBounds();
    if (bounds.left < 0.f)
        this->sprite.setPosition(bounds.width / 2.f, this->sprite.getPosition().y);
    if (bounds.left + bounds.width > target->getSize().x)
        this->sprite.setPosition(target->getSize().x - bounds.width / 2.f, this->sprite.getPosition().y);
    if (bounds.top < 0.f)
        this->sprite.setPosition(this->sprite.getPosition().x, bounds.height / 2.f);
    if (bounds.top + bounds.height > target->getSize().y)
        this->sprite.setPosition(this->sprite.getPosition().x, target->getSize().y - bounds.height / 2.f);
}

void Player::render(sf::RenderTarget& target) {
    // Rysujemy cząsteczki NAJPIERW, aby statek był nad nimi (zasłaniał je)
    for (auto& p : this->exhaustParticles) {
        target.draw(p.shape);
    }
    // Rysujemy statek
    target.draw(this->sprite);
}

const sf::Vector2f& Player::getPos() const { return this->sprite.getPosition(); }
float Player::getRotation() const { return this->sprite.getRotation(); }

void Player::stopSounds() {
    this->thrustSound.stop();
    this->reloadSound.stop();
}

void Player::resetPosition() {
    this->sprite.setPosition(640.f, 360.f);
    this->velocity = sf::Vector2f(0.f, 0.f);
    this->sprite.setRotation(0.f);
    this->sprite.setTexture(this->textureIdle);

    this->ammo = this->ammoMax;
    this->reloading = false;
    this->reloadTimer = 0;
    this->stopSounds();
    this->exhaustParticles.clear(); // Czyścimy dym przy resecie gry
}

void Player::startReload() {
    if (!this->reloading) {
        this->reloading = true;
        this->reloadTimer = 0;
        this->reloadSound.play();
    }
}

int Player::getAmmo() const { return this->ammo; }
int Player::getAmmoMax() const { return this->ammoMax; }
bool Player::isReloading() const { return this->reloading; }