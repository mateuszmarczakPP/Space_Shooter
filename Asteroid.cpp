#include "Asteroid.h"
#include <cmath>

Asteroid::Asteroid(
    sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed)
{
    this->shape.setTexture(*texture);
    this->shape.setPosition(pos_x, pos_y);
    this->shape.setScale(1.0f, 1.0f);
    sf::FloatRect bounds = this->shape.getLocalBounds();
    this->shape.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    this->direction = sf::Vector2f(dir_x, dir_y);
    this->movementSpeed = speed;
    this->hp = 1;          // Standardowa asteroida ginie na 1 hita
    this->pointsValue = 10; // Standardowa wartość punktowa
}

void Asteroid::update()
{
    this->shape.move(this->direction * this->movementSpeed);
    this->shape.rotate(1.2f);
}

void Asteroid::render(sf::RenderTarget *target)
{
    target->draw(this->shape);
}

const sf::FloatRect Asteroid::getBounds() const
{
    return this->shape.getGlobalBounds();
}

bool Asteroid::isOutOfBounds(const sf::RenderTarget &target) const
{
    sf::Vector2f pos = this->shape.getPosition();
    return (pos.y > target.getSize().y + 150.f || pos.y < -150.f || pos.x < -150.f
            || pos.x > target.getSize().x + 150.f);
}

bool Asteroid::takeDamage()
{
    this->hp--;
    return this->hp <= 0;
}

void Asteroid::collideWith(Asteroid *other)
{
    sf::Vector2f pos1 = this->shape.getPosition();
    sf::Vector2f pos2 = other->shape.getPosition();

    sf::Vector2f delta = pos1 - pos2;
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

    float r1 = this->getBounds().width / 2.f;
    float r2 = other->getBounds().width / 2.f;

    if (distance < r1 + r2) {
        if (distance == 0.f) {
            delta = sf::Vector2f(1.f, 0.f);
            distance = 1.f;
        }

        float overlap = (r1 + r2) - distance;
        sf::Vector2f normal = delta / distance;

        this->shape.move(normal * (overlap / 2.f));
        other->shape.move(-normal * (overlap / 2.f));

        sf::Vector2f tempDir = this->direction;
        this->direction = other->direction;
        other->direction = tempDir;
    }
}