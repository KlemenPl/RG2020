//
// Created by klemen on 09/12/2020.
//

#include "Sprite.h"

Sprite::Sprite( Vec2f position,  Vec2f movement,  Vec2f size,  Texture2D texture) : position(
        position), movement(movement), size(size), texture(texture)
{}

void Sprite::draw(Renderer2D &renderer2D)
{
    renderer2D.draw(texture, position, size);
}

void Sprite::update(float dt)
{
    position += movement * dt;

}
