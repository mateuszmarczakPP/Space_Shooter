#include "Bullet.h"

Bullet::Bullet(sf::Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float speed)
{
    this->shape.setTexture(*texture);
    this->shape.setPosition(pos_x, pos_y);
    this->shape.setScale(0.1f, 0.1f);
    sf::FloatRect bounds = this->shape.getLocalBounds();
    this->shape.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    this->direction = sf::Vector2f(dir_x, dir_y);
    this->movementSpeed = speed;
}

void Bullet::update()
{
    this->shape.move(this->direction * this->movementSpeed);
}
void Bullet::render(sf::RenderTarget *target)
{
    target->draw(this->shape);
}
const sf::FloatRect Bullet::getBounds() const
{
    return this->shape.getGlobalBounds();
}
bool Bullet::isOutOfBounds(const sf::RenderTarget &target) const
{
    sf::Vector2f pos = this->shape.getPosition();
    return (pos.x < 0.f || pos.x > target.getSize().x || pos.y < 0.f || pos.y > target.getSize().y);
}