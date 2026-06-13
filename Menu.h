#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Menu {
private:
    int selectedItemIndex;
    sf::Text menuOptions[4]; // Zmienione na 4 opcje
    bool hasSave;

public:
    Menu(float width, float height, sf::Font &font);
    void draw(sf::RenderWindow &window);
    void moveUp();
    void moveDown();
    int getPressedItem() { return selectedItemIndex; }
    void updateMouseHover(sf::Vector2f mousePos);
    int getClickedItem(sf::Vector2f mousePos);

    // NOWE: Funkcje sprawdzające czy jest zapis
    void checkSave();
    bool getHasSave() const { return hasSave; }
};

#endif