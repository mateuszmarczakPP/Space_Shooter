#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // NOWE: Wymagane dla dźwięku

class Explosion {
private:
    sf::Sprite shape;
    int lifetimeFrames;
    int maxLifetimeFrames;

public:
    Explosion(sf::Texture* texture, float pos_x, float pos_y);
    void update();
    void render(sf::RenderTarget* target);
    bool isExpired() const;
};

#endif
