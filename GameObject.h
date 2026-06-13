#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>

class GameObject {
protected:
    bool dead = false; // Flaga oznaczająca obiekt do usunięcia z pamięci
public:
    virtual ~GameObject() = default;

    // Czyste funkcje wirtualne - wymuszają na klasach pochodnych ich nadpisanie
    virtual void update(sf::RenderTarget* target) = 0;
    virtual void render(sf::RenderTarget* target) = 0;
    virtual const sf::FloatRect getBounds() const = 0;

    bool isDead() const { return dead; }
    virtual void kill() { dead = true; }
};

#endif