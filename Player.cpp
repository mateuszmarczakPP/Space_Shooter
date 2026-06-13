#include "Player.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

Player::Player() {
    this->loadConfig();
    this->velocity = sf::Vector2f(0.f, 0.f);
    this->shootTimer = this->shootTimerMax;
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
    if (!this->shootBuffer.loadFromFile("shoot.wav")) std::cout << "ERROR::PLAYER::Nie mozna wczytac shoot.wav\n";
    this->shootSound.setBuffer(this->shootBuffer);
    this->shootSound.setVolume(50.f);

    if (!this->thrustBuffer.loadFromFile("thrust.wav")) std::cout << "ERROR::PLAYER::Nie mozna wczytac thrust.wav\n";
    this->thrustSound.setBuffer(this->thrustBuffer);
    this->thrustSound.setLoop(true);
    this->thrustSound.setVolume(50.f);

    if (!this->reloadBuffer.loadFromFile("reload.wav")) std::cout << "ERROR::PLAYER::Nie mozna wczytac reload.wav\n";
    this->reloadSound.setBuffer(this->reloadBuffer);
    this->reloadSound.setVolume(50.f);
}

void Player::loadConfig() {
    std::ifstream configFile("config.txt");
    this->acceleration = 0.6f; this->drag = 0.9f; this->maxVelocity = 10.f; this->rotationSpeed = 4.f; this->shootTimerMax = 30;
    if (configFile.is_open()) {
        std::string key; float value;
        while (configFile >> key >> value) {
            if (key == "acceleration") this->acceleration = value;
            else if (key == "drag") this->drag = value;
            else if (key == "maxVelocity") this->maxVelocity = value;
            else if (key == "rotationSpeed") this->rotationSpeed = value;
            else if (key == "shootTimerMax") this->shootTimerMax = static_cast<int>(value);
        }
        configFile.close();
    }
}

void Player::initTexture() {
    this->textureIdle.loadFromFile("ship.png");
    this->textureThrust.loadFromFile("ship_thrust.png");
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
    if (this->ammo <= 0) { this->startReload(); return false; }
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
    return new Bullet(bulletTexture, this->sprite.getPosition().x, this->sprite.getPosition().y, std::cos(angle), std::sin(angle), 15.f);
}

void Player::update(sf::RenderTarget* target) {
    if (this->reloading) {
        this->reloadTimer++;
        if (this->reloadTimer >= this->reloadTimerMax) { this->ammo = this->ammoMax; this->reloading = false; this->reloadTimer = 0; }
    } else {
        if (this->shootTimer < this->shootTimerMax) this->shootTimer++;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && this->ammo < this->ammoMax) this->startReload();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) this->sprite.rotate(-this->rotationSpeed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) this->sprite.rotate(this->rotationSpeed);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        this->sprite.setTexture(this->textureThrust);
        if (this->thrustSound.getStatus() != sf::Sound::Playing) this->thrustSound.play();

        float angle = (this->sprite.getRotation() - 90.f) * 3.14159265f / 180.f;
        this->velocity.x += std::cos(angle) * this->acceleration;
        this->velocity.y += std::sin(angle) * this->acceleration;

        for (int i = 0; i < 2; i++) {
            ExhaustParticle p;
            p.shape.setSize(sf::Vector2f(8.f, 8.f)); p.shape.setOrigin(4.f, 4.f);
            float backX = this->sprite.getPosition().x - std::cos(angle) * 25.f + (rand() % 9 - 4);
            float backY = this->sprite.getPosition().y - std::sin(angle) * 25.f + (rand() % 9 - 4);
            p.shape.setPosition(backX, backY);
            p.velocity.x = -std::cos(angle) * (1.f + (rand() % 10) * 0.1f);
            p.velocity.y = -std::sin(angle) * (1.f + (rand() % 10) * 0.1f);
            p.maxLifetime = 15 + rand() % 15; p.lifetime = p.maxLifetime;
            p.shape.setFillColor(sf::Color(255, 150 + rand() % 105, 0, 255));
            this->exhaustParticles.push_back(p);
        }
    } else {
        this->sprite.setTexture(this->textureIdle);
        if (this->thrustSound.getStatus() == sf::Sound::Playing) this->thrustSound.stop();
    }

    for (int i = 0; i < this->exhaustParticles.size(); i++) {
        this->exhaustParticles[i].shape.move(this->exhaustParticles[i].velocity);
        this->exhaustParticles[i].lifetime--;
        float progress = static_cast<float>(this->exhaustParticles[i].lifetime) / this->exhaustParticles[i].maxLifetime;
        this->exhaustParticles[i].shape.setScale(progress, progress);
        sf::Color c = this->exhaustParticles[i].shape.getFillColor();
        c.a = static_cast<sf::Uint8>(255 * progress);
        c.r = static_cast<sf::Uint8>(255 * progress + 80 * (1.0f - progress));
        c.g = static_cast<sf::Uint8>(150 * progress + 80 * (1.0f - progress));
        c.b = static_cast<sf::Uint8>(0 * progress + 80 * (1.0f - progress));
        this->exhaustParticles[i].shape.setFillColor(c);
        if (this->exhaustParticles[i].lifetime <= 0) { this->exhaustParticles.erase(this->exhaustParticles.begin() + i); i--; }
    }

    float speed = std::sqrt(this->velocity.x * this->velocity.x + this->velocity.y * this->velocity.y);
    if (speed > this->maxVelocity) this->velocity = (this->velocity / speed) * this->maxVelocity;
    this->velocity *= this->drag;
    this->sprite.move(this->velocity);

    if (target) {
        sf::FloatRect bounds = this->sprite.getGlobalBounds();
        if (bounds.left < 0.f) this->sprite.setPosition(bounds.width / 2.f, this->sprite.getPosition().y);
        if (bounds.left + bounds.width > target->getSize().x) this->sprite.setPosition(target->getSize().x - bounds.width / 2.f, this->sprite.getPosition().y);
        if (bounds.top < 0.f) this->sprite.setPosition(this->sprite.getPosition().x, bounds.height / 2.f);
        if (bounds.top + bounds.height > target->getSize().y) this->sprite.setPosition(this->sprite.getPosition().x, target->getSize().y - bounds.height / 2.f);
    }
}

void Player::render(sf::RenderTarget* target) {
    if (target) {
        for (auto& p : this->exhaustParticles) target->draw(p.shape);
        target->draw(this->sprite);
    }
}

const sf::Vector2f& Player::getPos() const { return this->sprite.getPosition(); }
float Player::getRotation() const { return this->sprite.getRotation(); }
void Player::stopSounds() { this->thrustSound.stop(); this->reloadSound.stop(); }

void Player::resetPosition() {
    this->sprite.setPosition(640.f, 360.f);
    this->velocity = sf::Vector2f(0.f, 0.f);
    this->sprite.setRotation(0.f);
    this->sprite.setTexture(this->textureIdle);
    this->ammo = this->ammoMax;
    this->reloading = false;
    this->reloadTimer = 0;
    this->stopSounds();
    this->exhaustParticles.clear();
}

void Player::startReload() {
    if (!this->reloading) { this->reloading = true; this->reloadTimer = 0; this->reloadSound.play(); }
}

int Player::getAmmo() const { return this->ammo; }
int Player::getAmmoMax() const { return this->ammoMax; }
bool Player::isReloading() const { return this->reloading; }

void Player::loadState(int loadedAmmo, float px, float py) {
    this->ammo = loadedAmmo;
    this->sprite.setPosition(px, py);
    this->velocity = sf::Vector2f(0.f, 0.f);
    this->sprite.setRotation(0.f);
    this->exhaustParticles.clear();
}