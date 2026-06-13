#ifndef TIMEBONUS_HPP
#define TIMEBONUS_HPP

#include <SFML/Graphics.hpp>
#include "GameObject.h"

class TimeBonus : public GameObject
{
private:
    sf::Sprite shape;
    int lifetimeFrames;
    int maxLifetimeFrames;

public:
    TimeBonus(sf::Texture *texture, float pos_x, float pos_y);
    void update(sf::RenderTarget* target) override;
    void render(sf::RenderTarget *target) override;
    const sf::FloatRect getBounds() const override;
};

#endif