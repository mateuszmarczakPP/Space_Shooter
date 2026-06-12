#include "Explosion.h"
#include <cmath>

Explosion::Explosion(sf::Texture *texture, float pos_x, float pos_y)
{
    this->shape.setTexture(*texture);
    this->shape.setPosition(pos_x, pos_y);

    // Ustawienie na środku, aby wybuch rósł od środka
    sf::FloatRect bounds = this->shape.getLocalBounds();
    this->shape.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    this->lifetimeFrames = 0;
    // Wybuch żyje krótko: np. 30 klatek = 0.5s przy 60fps
    this->maxLifetimeFrames = 30;
}

void Explosion::update()
{
    this->lifetimeFrames++;

    float progress = static_cast<float>(this->lifetimeFrames)
                     / static_cast<float>(this->maxLifetimeFrames);

    // NOWE: EFEKT WIZUALNY WYBUCHU
    // 1. Skalowanie: Wybuch rośnie szybko na początku
    float baseScale = 0.5f; // Jeśli masz wielki obrazek, zmniejsz to
    float scale = baseScale + 0.8f * std::sin(progress * 1.57f);
    this->shape.setScale(scale, scale);

    // 2. Zanikanie: Staje się przezroczysty pod koniec
    int opacity = static_cast<int>(255.f * (1.0f - progress));
    sf::Color currentColor = this->shape.getColor();
    this->shape.setColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, opacity));
}

void Explosion::render(sf::RenderTarget *target)
{
    target->draw(this->shape);
}

bool Explosion::isExpired() const
{
    return this->lifetimeFrames >= this->maxLifetimeFrames;
}