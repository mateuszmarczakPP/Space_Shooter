#include "Asteroid.h"

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
void Asteroid::render(sf::RenderTarget* target) { target->draw(this->shape); }
const sf::FloatRect Asteroid::getBounds() const { return this->shape.getGlobalBounds(); }
bool Asteroid::isOutOfBounds(const sf::RenderTarget& target) const {
    sf::Vector2f pos = this->shape.getPosition();
    return (pos.y > target.getSize().y + 150.f || pos.y < -150.f ||
        pos.x < -150.f || pos.x > target.getSize().x + 150.f);
}