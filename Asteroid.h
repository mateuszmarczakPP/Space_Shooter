#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <SFML/Graphics.hpp>
#include "GameObject.h" // DZIEDZICZENIE

class Asteroid : public GameObject
{
protected:
    sf::Sprite shape;
    sf::Vector2f direction;
    float movementSpeed;
    int hp;
    int pointsValue;

public:
    Asteroid(sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed);

    // Implementacja czystych funkcji wirtualnych z GameObject
    void update(sf::RenderTarget* target) override;
    void render(sf::RenderTarget* target) override;
    const sf::FloatRect getBounds() const override;

    void collideWith(Asteroid *other);
    int getPointsValue() const { return this->pointsValue; }
    virtual bool takeDamage();
};

// --- POLIMORFIZM ---
class AsteroidFast : public Asteroid
{
public:
    AsteroidFast(sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed)
        : Asteroid(texture, pos_x, pos_y, dir_x, dir_y, speed * 1.8f) {
        this->shape.setScale(0.5f, 0.5f);
        this->pointsValue = 25;
    }

    void update(sf::RenderTarget* target) override {
        this->shape.move(this->direction * this->movementSpeed);
        this->shape.rotate(4.5f);
        if (target) {
            sf::Vector2f pos = this->shape.getPosition();
            if (pos.y > target->getSize().y + 150.f || pos.y < -150.f || pos.x < -150.f || pos.x > target->getSize().x + 150.f) this->kill();
        }
    }
};

class AsteroidArmored : public Asteroid
{
public:
    AsteroidArmored(sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed)
        : Asteroid(texture, pos_x, pos_y, dir_x, dir_y, speed * 0.6f) {
        this->shape.setScale(1.6f, 1.6f);
        this->hp = 3;
        this->pointsValue = 40;
        this->shape.setColor(sf::Color(150, 150, 150));
    }

    bool takeDamage() override {
        this->hp--;
        if (this->hp == 2) this->shape.setColor(sf::Color(200, 100, 100));
        else if (this->hp == 1) this->shape.setColor(sf::Color(255, 50, 50));
        return this->hp <= 0;
    }
};

#endif