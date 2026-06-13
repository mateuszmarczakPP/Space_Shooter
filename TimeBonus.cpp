#include "TimeBonus.h"
#include <cmath>

TimeBonus::TimeBonus(sf::Texture *texture, float pos_x, float pos_y) {
    this->shape.setTexture(*texture);
    this->shape.setPosition(pos_x, pos_y);
    sf::FloatRect bounds = this->shape.getLocalBounds();
    this->shape.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    this->lifetimeFrames = 0;
    this->maxLifetimeFrames = 420;
}

void TimeBonus::update(sf::RenderTarget* target) {
    this->lifetimeFrames++;
    float targetSize = 0.35f;
    float scale = targetSize + 0.03f * std::sin(this->lifetimeFrames * 0.1f);
    this->shape.setScale(scale, scale);
    this->shape.rotate(2.0f);

    if (this->lifetimeFrames >= this->maxLifetimeFrames) {
        this->kill();
    }
}

void TimeBonus::render(sf::RenderTarget *target) {
    if (target) target->draw(this->shape);
}

const sf::FloatRect TimeBonus::getBounds() const {
    return this->shape.getGlobalBounds();
}