#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

// Klasy
#include "Asteroid.h"
#include "Bullet.h"
#include "Explosion.h"
#include "Menu.h"
#include "Player.h"
#include "ScoreManager.h"
#include "TimeBonus.h"

// Typ enum określający w jakim stanie znajduje się gra
enum GameState { MENU, NICKNAME_INPUT, GAME, HIGHSCORES, GAME_OVER };

int main()
{
    // Inicjalizacja generatora liczb losowych
    srand(static_cast<unsigned>(time(NULL)));

    // KONFIGURACJA OKNA (Pełny ekran + skalowanie)
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(),
                            "Space Shooter Pro",
                            sf::Style::Fullscreen);
    sf::View view(
        sf::FloatRect(0.f, 0.f, 1280.f, 720.f)); // Gra wymusza wewnętrzną przestrzeń 1280x720
    window.setView(view);
    window.setFramerateLimit(60); // Stałe 60 klatek na sekundę

    // ŁADOWANIE ZASOBÓW
    sf::Font mainFont;
    if (!mainFont.loadFromFile("font.ttf")) {
        std::cout << "KRYTYCZNY BLAD: Nie mozna wczytac czcionki!\n";
    }

    sf::Texture texBullet, texAst, texBg, texBonus, texExplosion;
    texBullet.loadFromFile("bullet.png");
    texAst.loadFromFile("asteroid.png");
    texBg.loadFromFile("background.png");
    texBonus.loadFromFile("time_bonus.png");
    texExplosion.loadFromFile("explosion.png");

    // DŹWIĘK WYBUCHU ASTEROIDY
    sf::SoundBuffer explosionBuffer;
    if (!explosionBuffer.loadFromFile("explosion.wav")) {
        std::cout << "ERROR::SOUND::Nie mozna wczytac explosion.wav!" << std::endl;
    }
    sf::Sound explosionSound;
    explosionSound.setBuffer(explosionBuffer);
    explosionSound.setVolume(50.f);

    // DŹWIĘK KOLIZJI STATKU (KATASTROFA)
    sf::SoundBuffer crashBuffer;
    if (!crashBuffer.loadFromFile("crash.wav")) {
        std::cout << "ERROR::SOUND::Nie mozna wczytac crash.wav!" << std::endl;
    }
    sf::Sound crashSound;
    crashSound.setBuffer(crashBuffer);
    crashSound.setVolume(70.f);

    // Skalowanie tła do rozmiaru okna
    sf::Sprite background(texBg);
    background.setScale(1280.f / texBg.getSize().x, 720.f / texBg.getSize().y);

    // PRZYGOTOWANIE TEKSTÓW UI
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

    // Tworzenie graficznego przycisku powrotu dla tabeli wyników
    sf::Text backButtonText("[ POWROT ]", mainFont, 35);
    backButtonText.setFillColor(sf::Color::White);
    sf::FloatRect backBounds = backButtonText.getLocalBounds();
    backButtonText.setOrigin(backBounds.left + backBounds.width / 2.0f,
                             backBounds.top + backBounds.height / 2.0f);
    backButtonText.setPosition(640.f, 630.f); // Wyśrodkowany na dole ekranu

    // INICJALIZACJA OBIEKTÓW I ZMIENNYCH
    GameState state = MENU;
    Menu menu(1280.f, 720.f, mainFont);
    Player player;
    std::vector<Bullet *> bullets;
    std::vector<Asteroid *> asteroids;
    std::vector<TimeBonus *> timeBonuses;
    std::vector<Explosion *> explosions;

    std::string playerNick = "";
    int score = 0;
    bool playerHit = false;
    float gameTimeMax = 30.f;
    sf::Clock gameClock;
    int spawnTimer = 0;
    int timeBonusSpawnTimer = 0;

    // Data
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
            if (event.type == sf::Event::Closed)
                window.close();

            // OBSŁUGA MENU GŁÓWNEGO
            if (state == MENU) {
                // Konwersja pozycji myszy z monitora na układ współrzędnych naszej gry
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // Aktualizuj podświetlenie tekstu jeśli najeżdża myszka
                menu.updateMouseHover(mousePos);

                // --- OBSŁUGA KLIKNIĘCIA MYSZKĄ ---
                if (event.type == sf::Event::MouseButtonPressed
                    && event.mouseButton.button == sf::Mouse::Left) {
                    int choice = menu.getClickedItem(mousePos);
                    if (choice != -1) {
                        if (choice == 0) { // START
                            state = NICKNAME_INPUT;
                            playerNick = "";
                            inputNickText.setString("");
                        }
                        if (choice == 1) { // WYNIKI
                            state = HIGHSCORES;
                            highscoreListText.setString("TOP 10 WYNIKOW:\n\n"
                                                        + ScoreManager::loadAll());
                        }
                        if (choice == 2)
                            window.close(); // WYJŚCIE
                    }
                }
                // --- OBSŁUGA KLAWIATURY ---
                else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up)
                        menu.moveUp();
                    if (event.key.code == sf::Keyboard::Down)
                        menu.moveDown();
                    if (event.key.code == sf::Keyboard::Enter) {
                        int choice = menu.getPressedItem();
                        if (choice == 0) {
                            state = NICKNAME_INPUT;
                            playerNick = "";
                            inputNickText.setString("");
                        }
                        if (choice == 1) {
                            state = HIGHSCORES;
                            highscoreListText.setString("TOP 10 WYNIKOW:\n\n"
                                                        + ScoreManager::loadAll());
                        }
                        if (choice == 2)
                            window.close();
                    }
                }
            }

            // OBSŁUGA WPISYWANIA NICKU
            else if (state == NICKNAME_INPUT && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) {
                    if (!playerNick.empty())
                        playerNick.pop_back();
                } else if (event.text.unicode == 13) {
                    if (!playerNick.empty()) {
                        state = GAME;

                        crashSound.stop();
                        explosionSound.stop();

                        score = 0;
                        playerHit = false;
                        gameClock.restart();
                        gameTimeMax = 30.f;
                        timeBonusSpawnTimer = 0;
                        player.resetPosition();

                        // Czyszczenie obiektów
                        for (auto *a : asteroids)
                            delete a;
                        asteroids.clear();
                        for (auto *b : bullets)
                            delete b;
                        bullets.clear();
                        for (auto *tb : timeBonuses)
                            delete tb;
                        timeBonuses.clear();
                        for (auto *e : explosions)
                            delete e;
                        explosions.clear();
                    }
                } else if (event.text.unicode < 128 && playerNick.length() < 12) {
                    playerNick += static_cast<char>(event.text.unicode);
                }
                inputNickText.setString(playerNick + "_");
            }

            // POWRÓT Z GAME OVER
            else if (state == GAME_OVER && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    state = MENU;
            }

            // OBSŁUGA MYSZKI I KLAWIATURY W TABELI WYNIKÓW
            else if (state == HIGHSCORES) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect bounds = backButtonText.getGlobalBounds();

                // Łatwe najeżdżanie – powiększamy obszar aktywnego przycisku powrotu
                bounds.left -= 30.f;
                bounds.top -= 15.f;
                bounds.width += 60.f;
                bounds.height += 30.f;

                // Efekt podświetlenia (Hover)
                if (bounds.contains(mousePos)) {
                    backButtonText.setFillColor(sf::Color::Red);
                } else {
                    backButtonText.setFillColor(sf::Color::White);
                }

                // Kliknięcie myszką w przycisk powrotu
                if (event.type == sf::Event::MouseButtonPressed
                    && event.mouseButton.button == sf::Mouse::Left) {
                    if (bounds.contains(mousePos)) {
                        state = MENU;
                    }
                }
                // Klasyczny powrót za pomocą ESC
                else if (event.type == sf::Event::KeyPressed
                         && event.key.code == sf::Keyboard::Escape) {
                    state = MENU;
                }
            }
        }

        // --- LOGIKA GRY ---
        if (state == GAME) {
            float timeLeft = gameTimeMax - gameClock.getElapsedTime().asSeconds();

            // --- SEKRECYJNY PUNKT: KONIEC GRY ---
            if (timeLeft <= 0 || playerHit) {
                state = GAME_OVER;

                // Natychmiast wyciszamy dźwięk silnika (thrust) i przeładowania gracza!
                player.stopSounds();

                ScoreManager::save(playerNick, score, dateString);

                std::stringstream ss;
                ss << (playerHit ? "STATEK ZNISZCZONY!" : "KONIEC CZASU!") << "\nWYNIK: " << score
                   << "\n\nESC - MENU";
                gameOverText.setString(ss.str());

                // Wyśrodkowanie tekstu końca gry
                sf::FloatRect tr = gameOverText.getLocalBounds();
                gameOverText.setOrigin(tr.width / 2.f, tr.height / 2.f);
                gameOverText.setPosition(640.f, 360.f);
            }
            // ZMIANA: Wykonujemy resztę logiki TYLKO wtedy, gdy gra się nie zakończyła!
            else {
                // Górny pasek i amunicja
                std::stringstream ssUI;
                ssUI << "GRACZ: " << playerNick << "\nPUNKTY: " << score
                     << "\nCZAS: " << (int) timeLeft << "s";

                if (player.isReloading()) {
                    ssUI << "\nAMUNICJA: PRZELADOWYWANIE...";
                } else {
                    ssUI << "\nAMUNICJA: " << player.getAmmo() << " / " << player.getAmmoMax();
                }
                uiText.setString(ssUI.str());

                player.update(&window);

                // Strzelanie
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && player.canShoot()) {
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

                // SYSTEM SPAWNOWANIA CZASÓWEK
                if (++timeBonusSpawnTimer >= 600) {
                    float px = rand() % 1180 + 50.f;
                    float py = rand() % 620 + 50.f;
                    timeBonuses.push_back(new TimeBonus(&texBonus, px, py));
                    timeBonusSpawnTimer = 0;
                }

                // AKTUALIZACJA I WYKRYWANIE CZASÓWEK
                for (int i = 0; i < timeBonuses.size(); i++) {
                    timeBonuses[i]->update();

                    if (timeBonuses[i]->getBounds().intersects(player.getBounds())) {
                        gameTimeMax += 15.f;
                        delete timeBonuses[i];
                        timeBonuses.erase(timeBonuses.begin() + i--);
                        continue;
                    }

                    if (timeBonuses[i]->isExpired()) {
                        delete timeBonuses[i];
                        timeBonuses.erase(timeBonuses.begin() + i--);
                    }
                }

                // AKTUALIZACJA LOGIKI WYBUCHÓW
                for (int i = 0; i < explosions.size(); i++) {
                    explosions[i]->update();
                    if (explosions[i]->isExpired()) {
                        delete explosions[i];
                        explosions.erase(explosions.begin() + i--);
                    }
                }

                // SYSTEM SPAWNOWANIA ASTEROID
                if (++spawnTimer >= 35) {
                    float px, py, dx, dy;
                    int side = rand() % 4;
                    if (side == 0) {
                        px = rand() % 1280;
                        py = -50.f;
                        dx = (rand() % 100 - 50) / 50.f;
                        dy = 1.f;
                    } else if (side == 1) {
                        px = rand() % 1280;
                        py = 770.f;
                        dx = (rand() % 100 - 50) / 50.f;
                        dy = -1.f;
                    } else if (side == 2) {
                        px = -50.f;
                        py = rand() % 720;
                        dx = 1.f;
                        dy = (rand() % 100 - 50) / 50.f;
                    } else {
                        px = 1330.f;
                        py = rand() % 720;
                        dx = -1.f;
                        dy = (rand() % 100 - 50) / 50.f;
                    }

                    float speed = 2.f + static_cast<float>(rand() % 20) / 10.f;
                    asteroids.push_back(new Asteroid(&texAst, px, py, dx, dy, speed));
                    spawnTimer = 0;
                }

                // --- NOWE: ODBIJANIE ASTEROID OD SIEBIE ---
                // Podwójna pętla sprawdza każdą asteroidę z każdą inną
                for (int i = 0; i < asteroids.size(); i++) {
                    for (int j = i + 1; j < asteroids.size(); j++) {
                        asteroids[i]->collideWith(asteroids[j]);
                    }
                }

                // Kolizje i aktualizacja asteroid
                for (int i = 0; i < asteroids.size(); i++) {
                    asteroids[i]->update();

                    // Kolizja pocisk-asteroida
                    for (int j = 0; j < bullets.size(); j++) {
                        if (asteroids[i]->getBounds().intersects(bullets[j]->getBounds())) {
                            score += 10;

                            explosions.push_back(new Explosion(&texExplosion,
                                                               asteroids[i]->getBounds().left,
                                                               asteroids[i]->getBounds().top));
                            explosionSound.play();

                            delete asteroids[i];
                            asteroids.erase(asteroids.begin() + i--);
                            delete bullets[j];
                            bullets.erase(bullets.begin() + j);
                            goto next_ast;
                        }
                    }

                    // Kolizja gracz-asteroida
                    if (asteroids[i]->getBounds().intersects(player.getBounds())) {
                        crashSound.play();
                        explosions.push_back(
                            new Explosion(&texExplosion, player.getPos().x, player.getPos().y));
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
        }

        // --- RENDEROWANIE ---
        window.clear();
        window.draw(background);

        if (state == MENU)
            menu.draw(window);
        else if (state == NICKNAME_INPUT) {
            window.draw(promptText);
            window.draw(inputNickText);
        } else if (state == GAME) {
            for (auto *b : bullets)
                b->render(&window);
            for (auto *tb : timeBonuses)
                tb->render(&window);
            for (auto *e : explosions)
                e->render(&window);
            for (auto *a : asteroids)
                a->render(&window);

            if (!playerHit)
                player.render(window);

            window.draw(uiText);
        } else if (state == HIGHSCORES) {
            window.draw(highscoreListText);
            window.draw(backButtonText); // Rysujemy przycisk powrotu
        } else if (state == GAME_OVER)
            window.draw(gameOverText);

        window.display();
    }

    // CZYSZCZENIE PAMIĘCI
    for (auto *b : bullets)
        delete b;
    for (auto *a : asteroids)
        delete a;
    for (auto *tb : timeBonuses)
        delete tb;
    for (auto *e : explosions)
        delete e;

    return 0;
}