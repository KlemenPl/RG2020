//
// Created by klemen on 09/12/2020.
//

#include "Sprite.h"

Sprite::Sprite( glm::vec2 position,  glm::vec2 movement,  glm::vec2 size,  Ref<Texture2D> texture) : position(
        position), movement(movement), size(size), texture(texture)
{}

void Sprite::draw(Renderer2D &renderer2D)
{
    renderer2D.draw(*texture, position, size);
}

void Sprite::update(float dt)
{
    position += movement * dt;

}
