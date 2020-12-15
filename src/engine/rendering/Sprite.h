//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_SPRITE_H
#define TOWERDEFENSE_SPRITE_H


#include "Renderer2D.h"

class Sprite
{

public:
    glm::vec2 position;
    glm::vec2 movement;
    glm::vec2 size;
    Ref<Texture2D> texture;

    Sprite( glm::vec2 position, glm::vec2 movement,  glm::vec2 size,  Ref<Texture2D> texture);

    void update(float dt);

    void draw(Renderer2D &renderer2D);

};


#endif //TOWERDEFENSE_SPRITE_H
