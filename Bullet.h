#ifndef BULLET_HPP
#define BULLET_HPP

#include <SFML/Graphics.hpp>
#include "GameObject.h"

class Bullet : public GameObject
{
private:
    sf::Sprite shape;
    sf::Vector2f direction;
    float movementSpeed;

public:
    Bullet(sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed);
    void update(sf::RenderTarget* target) override;
    void render(sf::RenderTarget *target) override;
    const sf::FloatRect getBounds() const override;
};

#endif