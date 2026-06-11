#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <SFML/Graphics.hpp>

class Asteroid {
private:
    sf::Sprite shape;
    sf::Vector2f direction;
    float movementSpeed;

public:
    Asteroid(sf::Texture* texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed);
    void update();
    void render(sf::RenderTarget* target);
    const sf::FloatRect getBounds() const;
    bool isOutOfBounds(const sf::RenderTarget& target) const;

    // Funkcja odpowiedzialna za zderzenia między asteroidami
    void collideWith(Asteroid* other);
};

#endif