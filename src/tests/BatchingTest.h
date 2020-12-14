//
// Created by klemen on 12/12/2020.
//

#ifndef TOWERDEFENSE_BATCHINGTEST_H
#define TOWERDEFENSE_BATCHINGTEST_H

#include "Test.h"
#include <vector>
#include <sstream>
#include "../engine/rendering/Renderer2D.h"
#include "../engine/camera/OrthographicCamera.h"
#include "../engine/rendering/Sprite.h"
#include "../engine/graphics/BitmapFont.h"

/*
 * BunnyMark benchmark:
 *
 * Na moji opremi (Linux - Fedora 33, I5-4430, 16GB ram in GTX-1050TI) mi
 * lahko nariše okoli 62 tisoč premikajočih 2D teksturiranih objektov 60x
 * na sekundo brez problema. V testu je uporabljenih 31 različnih
 * tekstur (približno 160x160 pikslov na teksturo).
 *
 * Sem primerjal moj pristop risanja tudi z LibGDX framework in Godot
 * game enginom. Zelo presenetilo, ko sem ugotovil, da je moj pristop
 * risanja precej hitrejši :)
 *
 * Največje ozko grlo je po vsej verjetnosti CPU. Operacije z std::unordered_map
 * porabijo približno 40% časa v draw() funkciji. Uporaba grafične kartice
 * pa je zelo nizka (cca. 10%).
 *
 * Mogoče bi se še dalo dodatno optimizirati, da z uporabo geometrijskega
 * senčilnika pošljemo središče objekta, rotacijo in velikost in nato naj
 * senčilnik samodejno naredi ogljišča in jih pošlje naprej na naslednji
 * senčilnik.
 *
 */
class BatchingTest : public Test
{
public:
    void init() override;
    void start() override;
private:
    Renderer2D *renderer2D;
    OrthographicCamera *camera;

    std::stringstream ss;

    BitmapFont *font;

    float deltaTime{};
    float lastFrame{};

    const int maxSprites = 200000;
    int numberOfSprites = 10;
    std::vector<Sprite*> *sprites;


};


#endif //TOWERDEFENSE_BATCHINGTEST_H
