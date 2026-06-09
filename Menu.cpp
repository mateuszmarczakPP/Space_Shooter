#include "Menu.h"

Menu::Menu(float width, float height, sf::Font &font) {

    std::string options[] = { "GRAJ", "LISTA WYNIKOW", "WYJDZ" };

    for (int i = 0; i < 3; i++) {
        menuOptions[i].setFont(font);
        menuOptions[i].setString(options[i]);
        menuOptions[i].setCharacterSize(50);
        menuOptions[i].setFillColor(i == 0 ? sf::Color::Red : sf::Color::White); // Pierwsza opcja podœwietlona

        sf::FloatRect textRect = menuOptions[i].getLocalBounds();
        menuOptions[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        menuOptions[i].setPosition(sf::Vector2f(width / 2, height / (3 + 1) * (i + 1)));
    }
    selectedItemIndex = 0;
}

void Menu::draw(sf::RenderWindow& window) {
    for (int i = 0; i < 3; i++) {
        window.draw(menuOptions[i]);
    }
}

void Menu::moveUp() {
    if (selectedItemIndex - 1 >= 0) {
        menuOptions[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex--;
        menuOptions[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void Menu::moveDown() {
    if (selectedItemIndex + 1 < 3) {
        menuOptions[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex++;
        menuOptions[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}