#include "TimeBonus.h"
#include <cmath>

TimeBonus::TimeBonus(sf::Texture* texture, float pos_x, float pos_y) {
    this->shape.setTexture(*texture);
    this->shape.setPosition(pos_x, pos_y);
    
    // Ustawienie na środku obrazka by się ładnie obracał
    sf::FloatRect bounds = this->shape.getLocalBounds();
    this->shape.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    
    this->lifetimeFrames = 0;
    this->maxLifetimeFrames = 420; // 60 klatek * 7 sekund = znika po 7s
}

void TimeBonus::update() {
    this->lifetimeFrames++;
    
    // Efekt wizualny: pulsowanie (skalowanie) i obrót
    float scale = 1.0f + 0.15f * std::sin(this->lifetimeFrames * 0.1f);
    this->shape.setScale(scale, scale);
    this->shape.rotate(2.0f);
}

void TimeBonus::render(sf::RenderTarget* target) {
    target->draw(this->shape);
}

const sf::FloatRect TimeBonus::getBounds() const {
    return this->shape.getGlobalBounds();
}

bool TimeBonus::isExpired() const {
    return this->lifetimeFrames >= this->maxLifetimeFrames;
}