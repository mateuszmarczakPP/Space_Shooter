#include "Player.h"
#include <iostream>
#include <fstream>
#include <string>

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
    this->initAudio(); // <- NOWE: £adowanie dŸwiêków
}

Player::~Player() {}

void Player::initAudio() {
    // 1. DŸwiêk strza³u
    if (!this->shootBuffer.loadFromFile("shoot.wav"))
        std::cout << "ERROR::PLAYER::Nie mozna wczytac shoot.wav" << std::endl;
    this->shootSound.setBuffer(this->shootBuffer);
    this->shootSound.setVolume(50.f); // Nieco ciszej, bo strza³ów jest du¿o

    // 2. DŸwiêk silnika (lotu)
    if (!this->thrustBuffer.loadFromFile("thrust.wav"))
        std::cout << "ERROR::PLAYER::Nie mozna wczytac thrust.wav" << std::endl;
    this->thrustSound.setBuffer(this->thrustBuffer);
    this->thrustSound.setLoop(true); // Ustalamy, ¿e ten dŸwiêk ma siê zapêtlaæ!
    this->thrustSound.setVolume(50.f);

    // 3. DŸwiêk prze³adowania
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
        this->shootSound.play(); // <- NOWE: Odtwórz dŸwiêk strza³u
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
    // OBS£UGA TIMERA PRZE£ADOWANIA I KLAWISZA R
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

    // --- RUCH I POPRAWKA AUDIO DLA SILNIKA ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        this->sprite.setTexture(this->textureThrust);

        // NOWE: Odtwarzaj dŸwiêk silnika tylko jeœli ju¿ nie gra (unikamy j¹kania)
        if (this->thrustSound.getStatus() != sf::Sound::Playing) {
            this->thrustSound.play();
        }

        float angle = (this->sprite.getRotation() - 90.f) * 3.14159265f / 180.f;
        this->velocity.x += std::cos(angle) * this->acceleration;
        this->velocity.y += std::sin(angle) * this->acceleration;
    } else {
        this->sprite.setTexture(this->textureIdle);

        // NOWE: Zatrzymaj dŸwiêk silnika, gdy puszczamy klawisz W
        if (this->thrustSound.getStatus() == sf::Sound::Playing) {
            this->thrustSound.stop();
        }
    }

    float speed = std::sqrt(this->velocity.x * this->velocity.x + this->velocity.y * this->velocity.y);
    if (speed > this->maxVelocity) this->velocity = (this->velocity / speed) * this->maxVelocity;
    this->velocity *= this->drag;
    this->sprite.move(this->velocity);

    // BLOKADA WYJŒCIA POZA EKRAN
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

void Player::render(sf::RenderTarget& target) { target.draw(this->sprite); }
const sf::Vector2f& Player::getPos() const { return this->sprite.getPosition(); }
float Player::getRotation() const { return this->sprite.getRotation(); }

void Player::stopSounds() {
    // NOWE: Metoda uciszaj¹ca gracza
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
    this->stopSounds(); // NOWE: Ucisz dŸwiêki przy resecie
}

void Player::startReload() {
    if (!this->reloading) {
        this->reloading = true;
        this->reloadTimer = 0;
        this->reloadSound.play(); // <- NOWE: Odtwórz dŸwiêk prze³adowania
    }
}

int Player::getAmmo() const { return this->ammo; }
int Player::getAmmoMax() const { return this->ammoMax; }
bool Player::isReloading() const { return this->reloading; }