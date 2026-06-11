#ifndef TIMEBONUS_HPP
#define TIMEBONUS_HPP

#include <SFML/Graphics.hpp>

class TimeBonus {
private:
    sf::Sprite shape;
    int lifetimeFrames;
    int maxLifetimeFrames;

public:
    TimeBonus(sf::Texture* texture, float pos_x, float pos_y);
    void update();
    void render(sf::RenderTarget* target);
    const sf::FloatRect getBounds() const;
    bool isExpired() const;
};

#endif