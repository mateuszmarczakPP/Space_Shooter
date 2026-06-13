#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>    // unique_ptr
#include <algorithm> // remove_if
#include <cstdio>    // funkcja remove (usuwanie pliku)

#include "GameObject.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "Explosion.h"
#include "Menu.h"
#include "Player.h"
#include "ScoreManager.h"
#include "TimeBonus.h"

enum GameState { MENU, NICKNAME_INPUT, GAME, HIGHSCORES, GAME_OVER };

int main() {
    srand(static_cast<unsigned>(time(NULL)));

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Space Shooter Pro", sf::Style::Fullscreen);
    sf::View defaultView(sf::FloatRect(0.f, 0.f, 1280.f, 720.f));
    window.setView(defaultView);
    window.setFramerateLimit(60);

    sf::Font mainFont;
    if (!mainFont.loadFromFile("font.ttf")) std::cout << "KRYTYCZNY BLAD: Nie mozna wczytac czcionki!\n";

    sf::Texture texBullet, texAst, texBg, texBonus, texExplosion;
    texBullet.loadFromFile("bullet.png"); texAst.loadFromFile("asteroid.png");
    texBg.loadFromFile("background.png"); texBonus.loadFromFile("time_bonus.png"); texExplosion.loadFromFile("explosion.png");

    sf::SoundBuffer explosionBuffer, crashBuffer;
    explosionBuffer.loadFromFile("explosion.wav"); crashBuffer.loadFromFile("crash.wav");
    sf::Sound explosionSound(explosionBuffer); explosionSound.setVolume(50.f);
    sf::Sound crashSound(crashBuffer); crashSound.setVolume(70.f);

    sf::Sprite background(texBg);
    background.setScale(1280.f / texBg.getSize().x, 720.f / texBg.getSize().y);

    sf::Text uiText("", mainFont, 25); uiText.setPosition(20.f, 20.f);
    sf::Text promptText("WPISZ SWOJ NICK I NACISNIJ ENTER:", mainFont, 40); promptText.setPosition(300.f, 300.f);
    sf::Text inputNickText("", mainFont, 40); inputNickText.setFillColor(sf::Color::Yellow); inputNickText.setPosition(500.f, 380.f);
    sf::Text gameOverText("", mainFont, 50); gameOverText.setFillColor(sf::Color::Red);
    sf::Text highscoreListText("", mainFont, 25); highscoreListText.setPosition(450.f, 100.f);

    sf::Text backButtonText("[ POWROT ]", mainFont, 35); backButtonText.setFillColor(sf::Color::White);
    sf::FloatRect backBounds = backButtonText.getLocalBounds();
    backButtonText.setOrigin(backBounds.left + backBounds.width / 2.0f, backBounds.top + backBounds.height / 2.0f);
    backButtonText.setPosition(640.f, 630.f);

    GameState state = MENU;
    Menu menu(1280.f, 720.f, mainFont);

    // --- REWOLUCJA: JEDEN KONTENER POLIMORFICZNY ---
    std::vector<std::unique_ptr<GameObject>> allObjects;

    std::string playerNick = "";
    int score = 0;
    bool playerHit = false;
    float gameTimeMax = 30.f;
    sf::Clock gameClock;
    int spawnTimer = 0;
    int timeBonusSpawnTimer = 0;
    int shakeFrames = 0;
    float shakeMagnitude = 0.f;

    std::time_t t = std::time(nullptr); std::tm tm{}; localtime_s(&tm, &t);
    std::ostringstream oss; oss << std::put_time(&tm, "%Y-%m-%d");
    std::string dateString = oss.str();

    // Wyciągnięcie gracza z kontenera na potrzebę np. zapisu
    auto getPlayer = [&]() -> Player* {
        for (auto& o : allObjects) {
            if (auto p = dynamic_cast<Player*>(o.get())) return p;
        }
        return nullptr;
    };

    // Auto-Save do pliku
    auto trySaveGame = [&]() {
        float currentLeft = gameTimeMax - gameClock.getElapsedTime().asSeconds();
        Player* p = getPlayer();
        if (p && !playerHit && currentLeft > 0) {
            std::ofstream f("save.txt");
            f << playerNick << "\n" << score << "\n" << currentLeft << "\n" << p->getAmmo() << "\n" << p->getPos().x << "\n" << p->getPos().y << "\n";
        }
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (state == GAME) trySaveGame();
                window.close();
            }

            // ESC w grze robi automatyczny SAVE i wraca do menu
            if (state == GAME && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                trySaveGame();
                state = MENU;
                menu.checkSave();
            }

            if (state == MENU) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                menu.updateMouseHover(mousePos);

                auto handleMenuChoice = [&](int choice) {
                    if (choice == 0) { // NOWA GRA
                        state = NICKNAME_INPUT;
                        playerNick = ""; inputNickText.setString("");
                        std::remove("save.txt");
                        menu.checkSave();
                    } else if (choice == 1 && menu.getHasSave()) { // KONTYNUUJ
                        state = GAME;
                        std::ifstream f("save.txt");
                        int savedAmmo; float px, py;
                        f >> playerNick >> score >> gameTimeMax >> savedAmmo >> px >> py;
                        f.close();

                        allObjects.clear();
                        auto p = std::make_unique<Player>();
                        p->loadState(savedAmmo, px, py);
                        allObjects.push_back(std::move(p));

                        playerHit = false; gameClock.restart();
                        crashSound.stop(); explosionSound.stop();
                    } else if (choice == 2) {
                        state = HIGHSCORES;
                        highscoreListText.setString("TOP 10 WYNIKOW:\n\n" + ScoreManager::loadAll());
                    } else if (choice == 3) window.close();
                };

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    int choice = menu.getClickedItem(mousePos);
                    if (choice != -1) handleMenuChoice(choice);
                } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    handleMenuChoice(menu.getPressedItem());
                } else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) menu.moveUp();
                    if (event.key.code == sf::Keyboard::Down) menu.moveDown();
                }
            }
            else if (state == NICKNAME_INPUT && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8 && !playerNick.empty()) playerNick.pop_back();
                else if (event.text.unicode == 13 && !playerNick.empty()) {
                    state = GAME;
                    score = 0; playerHit = false; gameTimeMax = 30.f;
                    gameClock.restart();
                    allObjects.clear();
                    allObjects.push_back(std::make_unique<Player>());
                    crashSound.stop(); explosionSound.stop();
                } else if (event.text.unicode < 128 && playerNick.length() < 12 && event.text.unicode != 8) {
                    playerNick += static_cast<char>(event.text.unicode);
                }
                inputNickText.setString(playerNick + "_");
            }
            else if (state == GAME_OVER && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                state = MENU; menu.checkSave();
            }
            else if (state == HIGHSCORES) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect bounds = backButtonText.getGlobalBounds();
                bounds.left -= 30.f; bounds.top -= 15.f; bounds.width += 60.f; bounds.height += 30.f;
                backButtonText.setFillColor(bounds.contains(mousePos) ? sf::Color::Red : sf::Color::White);

                if ((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && bounds.contains(mousePos)) ||
                    (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                    state = MENU;
                }
            }
        }

        if (state == GAME) {
            float timeLeft = gameTimeMax - gameClock.getElapsedTime().asSeconds();
            Player* player = getPlayer();

            if (timeLeft <= 0 || playerHit) {
                state = GAME_OVER;
                std::remove("save.txt"); // Niszczy save po przegranej
                if (player) player->stopSounds();
                ScoreManager::save(playerNick, score, dateString);

                std::stringstream ss;
                ss << (playerHit ? "STATEK ZNISZCZONY!" : "KONIEC CZASU!") << "\nWYNIK: " << score << "\n\nESC - MENU";
                gameOverText.setString(ss.str());
                sf::FloatRect tr = gameOverText.getLocalBounds();
                gameOverText.setOrigin(tr.width / 2.f, tr.height / 2.f);
                gameOverText.setPosition(640.f, 360.f);
            }
            else {
                std::stringstream ssUI;
                ssUI << "GRACZ: " << playerNick << "\nPUNKTY: " << score << "\nCZAS: " << (int)timeLeft << "s";
                if (player && !player->isDead()) {
                    if (player->isReloading()) ssUI << "\nAMUNICJA: PRZELADOWYWANIE...";
                    else ssUI << "\nAMUNICJA: " << player->getAmmo() << " / " << player->getAmmoMax();

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && player->canShoot()) {
                        allObjects.push_back(std::unique_ptr<GameObject>(player->shoot(&texBullet)));
                    }
                }
                uiText.setString(ssUI.str());

                if (++timeBonusSpawnTimer >= 600) {
                    allObjects.push_back(std::make_unique<TimeBonus>(&texBonus, rand() % 1180 + 50.f, rand() % 620 + 50.f));
                    timeBonusSpawnTimer = 0;
                }

                if (++spawnTimer >= 35) {
                    float px, py, dx, dy; int side = rand() % 4;
                    if (side == 0) { px = rand() % 1280; py = -50.f; dx = (rand() % 100 - 50) / 50.f; dy = 1.f; }
                    else if (side == 1) { px = rand() % 1280; py = 770.f; dx = (rand() % 100 - 50) / 50.f; dy = -1.f; }
                    else if (side == 2) { px = -50.f; py = rand() % 720; dx = 1.f; dy = (rand() % 100 - 50) / 50.f; }
                    else { px = 1330.f; py = rand() % 720; dx = -1.f; dy = (rand() % 100 - 50) / 50.f; }

                    float speed = 2.f + static_cast<float>(rand() % 20) / 10.f;
                    int typeChance = rand() % 100;
                    if (typeChance < 60) allObjects.push_back(std::make_unique<Asteroid>(&texAst, px, py, dx, dy, speed));
                    else if (typeChance < 85) allObjects.push_back(std::make_unique<AsteroidFast>(&texAst, px, py, dx, dy, speed));
                    else allObjects.push_back(std::make_unique<AsteroidArmored>(&texAst, px, py, dx, dy, speed));
                    spawnTimer = 0;
                }

                // AKTUALIZACJA WSZYSTKICH OBIEKTÓW
                for (auto& obj : allObjects) obj->update(&window);

                // --- SYSTEM KOLIZJI (dynamic_cast sprawia tu cuda) ---
                for (size_t i = 0; i < allObjects.size(); i++) {
                    Asteroid* a = dynamic_cast<Asteroid*>(allObjects[i].get());
                    if (!a || a->isDead()) continue;

                    // Ast vs Pocisk
                    for (size_t j = 0; j < allObjects.size(); j++) {
                        Bullet* b = dynamic_cast<Bullet*>(allObjects[j].get());
                        if (b && !b->isDead() && a->getBounds().intersects(b->getBounds())) {
                            b->kill();
                            if (a->takeDamage()) {
                                score += a->getPointsValue();
                                allObjects.push_back(std::make_unique<Explosion>(&texExplosion, a->getBounds().left, a->getBounds().top));
                                explosionSound.play();
                                shakeFrames = 8; shakeMagnitude = 6.f;
                                a->kill();
                            }
                        }
                    }

                    // Ast vs Ast
                    for (size_t j = i + 1; j < allObjects.size(); j++) {
                        Asteroid* a2 = dynamic_cast<Asteroid*>(allObjects[j].get());
                        if (a2 && !a2->isDead()) a->collideWith(a2);
                    }

                    // Ast vs Player
                    if (player && !player->isDead() && a->getBounds().intersects(player->getBounds())) {
                        crashSound.play();
                        allObjects.push_back(std::make_unique<Explosion>(&texExplosion, player->getPos().x, player->getPos().y));
                        playerHit = true;
                        player->kill();
                        shakeFrames = 25; shakeMagnitude = 20.f;
                    }
                }

                // Czasówka vs Player
                if (player && !player->isDead()) {
                    for (auto& obj : allObjects) {
                        TimeBonus* tb = dynamic_cast<TimeBonus*>(obj.get());
                        if (tb && !tb->isDead() && tb->getBounds().intersects(player->getBounds())) {
                            gameTimeMax += 15.f;
                            tb->kill();
                        }
                    }
                }

                // CZYSTKA Z PAMIĘCI (Usuwa martwe obiekty i pociski pożarte przez autodestrukcję)
                allObjects.erase(std::remove_if(allObjects.begin(), allObjects.end(),
                                                [](const std::unique_ptr<GameObject>& obj) { return obj->isDead(); }), allObjects.end());
            }
        }

        // --- EFEKT TRZĘSIENIA KAMERY ---
        sf::View currentView = defaultView;
        if (shakeFrames > 0 && state == GAME) {
            currentView.move((rand() % 200 - 100) / 100.f * shakeMagnitude, (rand() % 200 - 100) / 100.f * shakeMagnitude);
            shakeFrames--;
        }
        window.setView(currentView);

        window.clear();
        window.draw(background);

        if (state == MENU) menu.draw(window);
        else if (state == NICKNAME_INPUT) { window.draw(promptText); window.draw(inputNickText); }
        else if (state == GAME) {
            for (auto& obj : allObjects) obj->render(&window);
            window.draw(uiText);
        }
        else if (state == HIGHSCORES) { window.draw(highscoreListText); window.draw(backButtonText); }
        else if (state == GAME_OVER) window.draw(gameOverText);

        window.display();
    }
    return 0;
}