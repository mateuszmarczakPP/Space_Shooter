#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Menu {
private:
    int selectedItemIndex;
    sf::Text menuOptions[3];

public:
    Menu(float width, float height, sf::Font &font);
    void draw(sf::RenderWindow& window);
    void moveUp();
    void moveDown();
    int getPressedItem() { return selectedItemIndex; }
};

#endif