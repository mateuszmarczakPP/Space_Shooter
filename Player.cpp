#include "Player.h"
#include <iostream>
#include <fstream>
#include <string>

Player::Player() {
    this->loadConfig();
    this->velocity = sf::Vector2f(0.f, 0.f);
    this->shootTimer = this->shootTimerMax;

    this->initTexture();
    this->initSprite();
}

Player::~Player() {}

void Player::loadConfig() {
    std::ifstream configFile("config.txt");

    //gdyby sie nie wczytalo
    this->acceleration = 0.6f;
    this->drag = 0.9f;
    this->maxVelocity = 10.f;
    this->rotationSpeed = 4.f;
    this->shootTimerMax = 30;

    //cofig
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
    }
    else {
        std::cout << "Blad: Nie znaleziono config.txt. Uzywam ustawien domyslnych.\n";
    }
}

void Player::initTexture() {
    if (!this->texture.loadFromFile("ship.png"))
        std::cout << "ERROR::PLAYER::Could not load texture!" << std::endl;
}
//teksturka statku
void Player::initSprite() {
    this->sprite.setTexture(this->texture);
    this->sprite.setScale(0.3f, 0.3f);
    sf::FloatRect bounds = this->sprite.getLocalBounds();
    this->sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    this->sprite.setPosition(640.f, 360.f);
}
//sprawdzanie czy moze strzelac
bool Player::canShoot() {
    if (this->shootTimer >= this->shootTimerMax) {
        this->shootTimer = 0;
        return true;
    }
    return false;
}
//strzelanie
Bullet* Player::shoot(sf::Texture* bulletTexture) {
    float angle = (this->sprite.getRotation() - 90.f) * 3.14159265f / 180.f;
    return new Bullet(bulletTexture, this->sprite.getPosition().x, this->sprite.getPosition().y,
        std::cos(angle), std::sin(angle), 15.f);
}
// sterowanie statkiem
void Player::update(const sf::RenderTarget* target) {
    if (this->shootTimer < this->shootTimerMax) this->shootTimer++;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) this->sprite.rotate(-this->rotationSpeed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) this->sprite.rotate(this->rotationSpeed);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        float angle = (this->sprite.getRotation() - 90.f) * 3.14159265f / 180.f;
        this->velocity.x += std::cos(angle) * this->acceleration;
        this->velocity.y += std::sin(angle) * this->acceleration;
    }

    float speed = std::sqrt(this->velocity.x * this->velocity.x + this->velocity.y * this->velocity.y);
    if (speed > this->maxVelocity) this->velocity = (this->velocity / speed) * this->maxVelocity;
    this->velocity *= this->drag;
    this->sprite.move(this->velocity);

    // --- BLOKADA WYJSCIA POZA EKRAN ---
    sf::FloatRect bounds = this->sprite.getGlobalBounds();

    // Lewa
    if (bounds.left < 0.f)
        this->sprite.setPosition(bounds.width / 2.f, this->sprite.getPosition().y);

    // Prawa
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
void Player::resetPosition() {
    // Ustawienie na œrodku ekranu
    this->sprite.setPosition(640.f, 360.f);

    // Zerowanie pêdu
    this->velocity = sf::Vector2f(0.f, 0.f);

    // Reset rotacji
    this->sprite.setRotation(0.f);
}