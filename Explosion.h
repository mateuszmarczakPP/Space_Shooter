#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameObject.h"

class Explosion : public GameObject
{
private:
    sf::Sprite shape;
    int lifetimeFrames;
    int maxLifetimeFrames;

public:
    Explosion(sf::Texture *texture, float pos_x, float pos_y);
    void update(sf::RenderTarget* target) override;
    void render(sf::RenderTarget *target) override;
    const sf::FloatRect getBounds() const override { return this->shape.getGlobalBounds(); }
};

#endif