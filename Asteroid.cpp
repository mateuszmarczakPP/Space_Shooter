#include "Asteroid.h"
#include <cmath> // Wymagane do obliczania odległości (pierwiastków)

Asteroid::Asteroid(sf::Texture* texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed) {
    this->shape.setTexture(*texture);
    this->shape.setPosition(pos_x, pos_y);
    this->shape.setScale(1.0f, 1.0f);
    sf::FloatRect bounds = this->shape.getLocalBounds();
    this->shape.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    this->direction = sf::Vector2f(dir_x, dir_y);
    this->movementSpeed = speed;
}

void Asteroid::update() {
    this->shape.move(this->direction * this->movementSpeed);
    this->shape.rotate(1.2f);
}

void Asteroid::render(sf::RenderTarget* target) {
    target->draw(this->shape);
}

const sf::FloatRect Asteroid::getBounds() const {
    return this->shape.getGlobalBounds();
}

bool Asteroid::isOutOfBounds(const sf::RenderTarget& target) const {
    sf::Vector2f pos = this->shape.getPosition();
    return (pos.y > target.getSize().y + 150.f || pos.y < -150.f ||
            pos.x < -150.f || pos.x > target.getSize().x + 150.f);
}

// LOGIKA ZDERZEŃ MIĘDZY ASTEROIDAMI
void Asteroid::collideWith(Asteroid* other) {
    sf::Vector2f pos1 = this->shape.getPosition();
    sf::Vector2f pos2 = other->shape.getPosition();

    // Obliczanie wektora odległości między środkami asteroid
    sf::Vector2f delta = pos1 - pos2;
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

    // Przyjmujemy promień asteroidy jako połowę szerokości obrazka
    float r1 = this->getBounds().width / 2.f;
    float r2 = other->getBounds().width / 2.f;

    // Jeśli odległość jest mniejsza niż suma promieni, to znaczy, że w siebie uderzyły
    if (distance < r1 + r2) {

        // Zabezpieczenie na wypadek, gdyby idealnie zrespiły się w tym samym pikselu
        if (distance == 0.f) {
            delta = sf::Vector2f(1.f, 0.f);
            distance = 1.f;
        }

        // 1. ROZDZIELANIE
        // Przesuwamy je delikatnie w przeciwnych kierunkach, żeby się w sobie nie zaklinowały
        float overlap = (r1 + r2) - distance;
        sf::Vector2f normal = delta / distance; // Znormalizowany wektor kierunku

        this->shape.move(normal * (overlap / 2.f));
        other->shape.move(-normal * (overlap / 2.f));

        // 2. ODBICIE
        // Dla zachowania grywalności (arcade) po prostu zamieniamy się ich kierunkami
        sf::Vector2f tempDir = this->direction;
        this->direction = other->direction;
        other->direction = tempDir;
    }
}