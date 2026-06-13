#include "Explosion.h"
#include <cmath>

Explosion::Explosion(sf::Texture *texture, float pos_x, float pos_y) {
    this->shape.setTexture(*texture);
    this->shape.setPosition(pos_x, pos_y);
    sf::FloatRect bounds = this->shape.getLocalBounds();
    this->shape.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    this->lifetimeFrames = 0;
    this->maxLifetimeFrames = 30;
}

void Explosion::update(sf::RenderTarget* target) {
    this->lifetimeFrames++;
    float progress = static_cast<float>(this->lifetimeFrames) / static_cast<float>(this->maxLifetimeFrames);

    float baseScale = 0.5f;
    float scale = baseScale + 0.8f * std::sin(progress * 1.57f);
    this->shape.setScale(scale, scale);

    int opacity = static_cast<int>(255.f * (1.0f - progress));
    sf::Color currentColor = this->shape.getColor();
    this->shape.setColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, opacity));

    if (this->lifetimeFrames >= this->maxLifetimeFrames) {
        this->kill();
    }
}

void Explosion::render(sf::RenderTarget *target) {
    if (target) target->draw(this->shape);
}