#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

//klasy
#include "Player.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "Menu.h"
#include "ScoreManager.h"
#include "TimeBonus.h" // NOWE: dodana klasa czasówki

//typ enum określający w jakim stanie znajduje się gra
enum GameState { MENU, NICKNAME_INPUT, GAME, HIGHSCORES, GAME_OVER };

int main() {
    //Inicjalizacja generatora liczb losowych
    srand(static_cast<unsigned>(time(NULL)));

    //KONFIGURACJA OKNA
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Space Shooter Pro", sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60); // Stałe 60 klatek na sekundę

    //ŁADOWANIE ZASOBÓW
    sf::Font mainFont;
    if (!mainFont.loadFromFile("font.ttf")) {
        std::cout << "KRYTYCZNY BLAD: Nie mozna wczytac czcionki!\n";
    }

    sf::Texture texBullet, texAst, texBg, texBonus;
    texBullet.loadFromFile("bullet.png");
    texAst.loadFromFile("asteroid.png");
    texBg.loadFromFile("background.png");
    texBonus.loadFromFile("time_bonus.png"); // NOWE: wczytywanie tekstury czasówki

    // Skalowanie tła do rozmiaru okna
    sf::Sprite background(texBg);
    background.setScale(1280.f / texBg.getSize().x, 720.f / texBg.getSize().y);

    // 3. PRZYGOTOWANIE TEKSTÓW UI
    sf::Text uiText("", mainFont, 25);
    uiText.setPosition(20.f, 20.f);

    sf::Text promptText("WPISZ SWOJ NICK I NACISNIJ ENTER:", mainFont, 40);
    promptText.setPosition(300.f, 300.f);

    sf::Text inputNickText("", mainFont, 40);
    inputNickText.setFillColor(sf::Color::Yellow);
    inputNickText.setPosition(500.f, 380.f);

    sf::Text gameOverText("", mainFont, 50);
    gameOverText.setFillColor(sf::Color::Red);

    sf::Text highscoreListText("", mainFont, 25);
    highscoreListText.setPosition(450.f, 100.f);

    // 4. INICJALIZACJA OBIEKTÓW I ZMIENNYCH
    GameState state = MENU;
    Menu menu(1280.f, 720.f, mainFont);
    Player player;
    std::vector<Bullet*> bullets;
    std::vector<Asteroid*> asteroids;
    std::vector<TimeBonus*> timeBonuses; // NOWE: wektor przechowujący na planszy czasówki

    std::string playerNick = "";
    int score = 0;
    bool playerHit = false;
    float gameTimeMax = 30.f;
    sf::Clock gameClock;
    int spawnTimer = 0;
    int timeBonusSpawnTimer = 0; // NOWE: Timer spawnowania czasówki

    // data
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    std::string dateString = oss.str();

    // --- GŁÓWNA PĘTLA PROGRAMU ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // OBSŁUGA MENU GŁÓWNEGO
            if (state == MENU && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) menu.moveUp();
                if (event.key.code == sf::Keyboard::Down) menu.moveDown();
                if (event.key.code == sf::Keyboard::Enter) {
                    int choice = menu.getPressedItem();
                    if (choice == 0) { // START
                        state = NICKNAME_INPUT;
                        playerNick = "";
                        inputNickText.setString("");
                    }
                    if (choice == 1) { // WYNIKI
                        state = HIGHSCORES;
                        highscoreListText.setString("TOP 10 WYNIKOW:\n\n" + ScoreManager::loadAll() + "\n[ESC] POWROT");
                    }
                    if (choice == 2) window.close(); // WYJSCIE
                }
            }

            // OBSŁUGA WPISYWANIA NICKU
            else if (state == NICKNAME_INPUT && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) {
                    if (!playerNick.empty()) playerNick.pop_back();
                }
                else if (event.text.unicode == 13) {
                    if (!playerNick.empty()) {
                        state = GAME;
                        score = 0;
                        playerHit = false;
                        gameClock.restart(); // Reset czasu
                        gameTimeMax = 30.f;  // NOWE: resetowanie max czasu przy uruchomieniu gry po raz kolejny
                        timeBonusSpawnTimer = 0;
                        player.resetPosition(); // Reset statku

                        // NOWE: czyszczenie obiektów w przypadku restartu
                        for (auto* a : asteroids) delete a;
                        asteroids.clear();
                        for (auto* b : bullets) delete b;
                        bullets.clear();
                        for (auto* tb : timeBonuses) delete tb;
                        timeBonuses.clear();
                    }
                }
                else if (event.text.unicode < 128 && playerNick.length() < 12) {
                    playerNick += static_cast<char>(event.text.unicode);
                }
                inputNickText.setString(playerNick + "_");
            }

            // POWRÓT DO MENU
            else if ((state == GAME_OVER || state == HIGHSCORES) && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) state = MENU;
            }
        }

        // --- LOGIKA GRY ---
        if (state == GAME) {
            float timeLeft = gameTimeMax - gameClock.getElapsedTime().asSeconds();

            // Warunki końca gry
            if (timeLeft <= 0 || playerHit) {
                state = GAME_OVER;
                ScoreManager::save(playerNick, score, dateString);

                std::stringstream ss;
                ss << (playerHit ? "STATEK ZNISZCZONY!" : "KONIEC CZASU!")
                   << "\nWYNIK: " << score << "\n\nESC - MENU";
                gameOverText.setString(ss.str());

                // Wyśrodkowanie tekstu końca gry
                sf::FloatRect tr = gameOverText.getLocalBounds();
                gameOverText.setOrigin(tr.width / 2.f, tr.height / 2.f);
                gameOverText.setPosition(640.f, 360.f);
            }

            // Górny pasek
            std::stringstream ssUI;
            ssUI << "GRACZ: " << playerNick << "\nPUNKTY: " << score << "\nCZAS: " << (int)timeLeft << "s";
            uiText.setString(ssUI.str());

            player.update(&window);

            // Strzelanie
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && player.canShoot()) {
                score -= 5; // Każdy strzał kosztuje 5 pkt (balans gry)
                bullets.push_back(player.shoot(&texBullet));
            }

            // Aktualizacja pocisków i usuwanie tych poza ekranem
            for (int i = 0; i < bullets.size(); i++) {
                bullets[i]->update();
                if (bullets[i]->isOutOfBounds(window)) {
                    delete bullets[i];
                    bullets.erase(bullets.begin() + i--);
                }
            }

            // NOWE: SYSTEM SPAWNOWANIA CZASÓWEK
            // Czekamy 600 klatek (czyli równe 10 sekund przy 60 fps)
            if (++timeBonusSpawnTimer >= 600) {
                // Pojawia się w obszarze bezpiecznym od krawędzi (50 px do 1230 px)
                float px = rand() % 1180 + 50.f;
                float py = rand() % 620 + 50.f;
                timeBonuses.push_back(new TimeBonus(&texBonus, px, py));
                timeBonusSpawnTimer = 0;
            }

            // NOWE: AKTUALIZACJA I WYKRYWANIE CZASÓWEK
            for (int i = 0; i < timeBonuses.size(); i++) {
                timeBonuses[i]->update();

                // Jeżeli gracz najedzie na czasówkę (Kolizja)
                if (timeBonuses[i]->getBounds().intersects(player.getBounds())) {
                    gameTimeMax += 15.f; // Dodajemy 15s poprzez zwiększenie marginesu
                    delete timeBonuses[i];
                    timeBonuses.erase(timeBonuses.begin() + i--);
                    continue; // Przejście do następnego bonusu w pętli
                }

                // Usuwanie z mapy po 7 sekundach jeśli nie zbierze
                if (timeBonuses[i]->isExpired()) {
                    delete timeBonuses[i];
                    timeBonuses.erase(timeBonuses.begin() + i--);
                }
            }

            // SYSTEM SPAWNOWANIA ASTEROID
            if (++spawnTimer >= 35) {
                float px, py, dx, dy;
                int side = rand() % 4;
                // 0: Góra, 1: Dół, 2: Lewo, 3: Prawo
                if (side == 0) { px = rand() % 1280; py = -50.f; dx = (rand() % 100 - 50) / 50.f; dy = 1.f; }
                else if (side == 1) { px = rand() % 1280; py = 770.f; dx = (rand() % 100 - 50) / 50.f; dy = -1.f; }
                else if (side == 2) { px = -50.f; py = rand() % 720; dx = 1.f; dy = (rand() % 100 - 50) / 50.f; }
                else { px = 1330.f; py = rand() % 720; dx = -1.f; dy = (rand() % 100 - 50) / 50.f; }

                float speed = 2.f + static_cast<float>(rand() % 20) / 10.f;
                asteroids.push_back(new Asteroid(&texAst, px, py, dx, dy, speed));
                spawnTimer = 0;
            }

            // Kolizje i aktualizacja asteroid
            for (int i = 0; i < asteroids.size(); i++) {
                asteroids[i]->update();

                // Kolizja pocisk-asteroida
                for (int j = 0; j < bullets.size(); j++) {
                    if (asteroids[i]->getBounds().intersects(bullets[j]->getBounds())) {
                        score += 10;
                        delete asteroids[i]; asteroids.erase(asteroids.begin() + i--);
                        delete bullets[j]; bullets.erase(bullets.begin() + j);
                        goto next_ast; // Przejdź do kolejnej asteroidy (ta już nie istnieje)
                    }
                }

                // Kolizja gracz-asteroida
                if (asteroids[i]->getBounds().intersects(player.getBounds())) {
                    playerHit = true;
                }

                // Usuwanie asteroid poza ekranem
                if (asteroids[i]->isOutOfBounds(window)) {
                    delete asteroids[i];
                    asteroids.erase(asteroids.begin() + i--);
                }
            next_ast:;
            }
        }

        // --- RENDEROWANIE ---
        window.clear();
        window.draw(background);

        if (state == MENU) menu.draw(window);
        else if (state == NICKNAME_INPUT) {
            window.draw(promptText);
            window.draw(inputNickText);
        }
        else if (state == GAME) {
            for (auto* b : bullets) b->render(&window);
            for (auto* tb : timeBonuses) tb->render(&window); // NOWE: Rysowanie na ekranie
            for (auto* a : asteroids) a->render(&window);
            player.render(window);
            window.draw(uiText);
        }
        else if (state == HIGHSCORES) window.draw(highscoreListText);
        else if (state == GAME_OVER) window.draw(gameOverText);

        window.display();
    }

    // CZYSZCZENIE PAMIĘCI
    for (auto* b : bullets) delete b;
    for (auto* a : asteroids) delete a;
    for (auto* tb : timeBonuses) delete tb; // NOWE: Zapobieganie wyciekowi

    return 0;
}