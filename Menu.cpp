#include "Menu.h"
#include <fstream>

Menu::Menu(float width, float height, sf::Font &font) {
    std::string options[] = {"NOWA GRA", "KONTYNUUJ", "LISTA WYNIKOW", "WYJDZ"};

    for (int i = 0; i < 4; i++) {
        menuOptions[i].setFont(font);
        menuOptions[i].setString(options[i]);
        menuOptions[i].setCharacterSize(50);
        sf::FloatRect textRect = menuOptions[i].getLocalBounds();
        menuOptions[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        menuOptions[i].setPosition(sf::Vector2f(width / 2, height / (4 + 1) * (i + 1)));
    }
    selectedItemIndex = 0;
    menuOptions[0].setFillColor(sf::Color::Red);
    this->checkSave();
}

void Menu::checkSave() {
    std::ifstream file("save.txt");
    hasSave = file.good();
    file.close();

    // Szarzenie niedostępnej opcji
    if (!hasSave) {
        menuOptions[1].setFillColor(sf::Color(100, 100, 100));
        if (selectedItemIndex == 1) moveDown();
    } else if (selectedItemIndex != 1) {
        menuOptions[1].setFillColor(sf::Color::White);
    }
}

void Menu::draw(sf::RenderWindow &window) {
    for (int i = 0; i < 4; i++) window.draw(menuOptions[i]);
}

void Menu::moveUp() {
    if (selectedItemIndex - 1 >= 0) {
        menuOptions[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex--;
        if (!hasSave && selectedItemIndex == 1) selectedItemIndex--; // Przeskakiwanie zablokowanego
        menuOptions[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void Menu::moveDown() {
    if (selectedItemIndex + 1 < 4) {
        menuOptions[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex++;
        if (!hasSave && selectedItemIndex == 1) selectedItemIndex++; // Przeskakiwanie zablokowanego
        menuOptions[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void Menu::updateMouseHover(sf::Vector2f mousePos) {
    for (int i = 0; i < 4; i++) {
        if (!hasSave && i == 1) continue;
        sf::FloatRect bounds = menuOptions[i].getGlobalBounds();
        bounds.left -= 30.f; bounds.top -= 15.f; bounds.width += 60.f; bounds.height += 30.f;

        if (bounds.contains(mousePos)) {
            menuOptions[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex = i;
            menuOptions[selectedItemIndex].setFillColor(sf::Color::Red);
        }
    }
}

int Menu::getClickedItem(sf::Vector2f mousePos) {
    for (int i = 0; i < 4; i++) {
        if (!hasSave && i == 1) continue;
        sf::FloatRect bounds = menuOptions[i].getGlobalBounds();
        bounds.left -= 30.f; bounds.top -= 15.f; bounds.width += 60.f; bounds.height += 30.f;
        if (bounds.contains(mousePos)) return i;
    }
    return -1;
}