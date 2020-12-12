//
// Created by klemen on 12/12/2020.
//

#ifndef TOWERDEFENSE_BUNNYMARKTEST_H
#define TOWERDEFENSE_BUNNYMARKTEST_H

#include "Test.h"
#include <vector>
#include "../engine/rendering/Renderer2D.h"
#include "../engine/camera/OrthographicCamera.h"
#include "../engine/rendering/Sprite.h"
#include "../engine/graphics/BitmapFont.h"

/*
 * BunnyMark benchmark:
 *
 * Na moji opremi (Linux: I5-4430, 16GB ram and GTX-1050TI) mi
 * lahko nariše okoli 55 tisoč 2D teksturiranih objektov 60x
 * na sekundo brez problema.
 *
 * Največje ozko grlo je CPU, sicer pa je uporaba grafične
 * kartice zelo majhna.
 */
class BunnyMarkTest : public Test
{
private:
    Renderer2D* renderer2D;
    OrthographicCamera* camera;

    BitmapFont* font;

    float deltaTime{};
    float lastFrame{};

    int numberOfSprites = 10;
    std::vector<Sprite> sprites;
public:
    void init() override;
    void render() override;


};


#endif //TOWERDEFENSE_BUNNYMARKTEST_H
