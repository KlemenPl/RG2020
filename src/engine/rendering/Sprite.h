//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_SPRITE_H
#define TOWERDEFENSE_SPRITE_H


#include "Renderer2D.h"

class Sprite
{

public:
    Vec2f position;
    Vec2f movement;
    Vec2f size;
    Texture2D texture;

    Sprite(const Vec2f &position,const Vec2f& movement, const Vec2f &size, const Texture2D &texture);

    void update(float dt);

    void draw(Renderer2D &renderer2D);

};


#endif //TOWERDEFENSE_SPRITE_H
