#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <SFML/Graphics.hpp>

class Asteroid
{
protected:
    sf::Sprite shape;
    sf::Vector2f direction;
    float movementSpeed;
    int hp;          // Dodane HP dla obsługi pancerza
    int pointsValue; // Ile punktów daje ten konkretny typ

public:
    Asteroid(sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed);
    virtual ~Asteroid() = default; // Wirtualny destruktor dla bezpiecznego polimorfizmu

    virtual void update();
    void render(sf::RenderTarget *target);
    const sf::FloatRect getBounds() const;
    bool isOutOfBounds(const sf::RenderTarget &target) const;
    void collideWith(Asteroid *other);

    // Gettery potrzebne do logiki w main.cpp
    int getPointsValue() const { return this->pointsValue; }
    virtual bool takeDamage(); // Zwraca true, jeśli asteroida została zniszczona
};

// 1. NOWY TYP: Szybki Odłamek (Mniejszy, bardzo szybki, rotuje jak szalony)
class AsteroidFast : public Asteroid
{
public:
    AsteroidFast(sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed)
        : Asteroid(texture, pos_x, pos_y, dir_x, dir_y, speed * 1.8f) // Zwiększamy bazową prędkość
    {
        this->shape.setScale(0.5f, 0.5f); // Połowa normalnego rozmiaru
        this->pointsValue = 25;           // Więcej punktów za trudniejszy cel
    }

    void update() override
    {
        this->shape.move(this->direction * this->movementSpeed);
        this->shape.rotate(4.5f); // Kręci się znacznie szybciej
    }
};

// 2. NOWY TYP: Pancerna Asteroida (Duża, wolniejsza, ma 3 punkty życia)
class AsteroidArmored : public Asteroid
{
public:
    AsteroidArmored(sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed)
        : Asteroid(texture, pos_x, pos_y, dir_x, dir_y, speed * 0.6f) // Wolniejsza
    {
        this->shape.setScale(1.6f, 1.6f); // Większy rozmiar
        this->hp = 3;                     // Wymaga 3 strzałów
        this->pointsValue = 40;
        this->shape.setColor(sf::Color(150, 150, 150)); // Na start lekko ciemniejsza / szara
    }

    bool takeDamage() override
    {
        this->hp--;
        if (this->hp == 2) {
            this->shape.setColor(sf::Color(200, 100, 100)); // Ślady uszkodzeń (lekki czerwony)
        } else if (this->hp == 1) {
            this->shape.setColor(sf::Color(255, 50, 50));   // Poważne uszkodzenia (mocny czerwony)
        }
        return this->hp <= 0;
    }
};

#endif