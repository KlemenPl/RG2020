//
// Created by klemen on 14/12/2020.
//

#ifndef TOWERDEFENSE_TESTSCREEN_H
#define TOWERDEFENSE_TESTSCREEN_H


#include "Screen.h"
#include <sstream>
#include <vector>
#include "../../engine/Core.h"
#include "../../engine/rendering/Renderer2D.h"
#include "../../engine/camera/OrthographicCamera.h"
#include "../../engine/rendering/Sprite.h"

class TestScreen : public Screen
{
private:
    Renderer2D *renderer2D;
    OrthographicCamera *camera;

    std::stringstream ss;

    Ref<BitmapFont> font;

    float deltaTime{};
    float lastFrame{};

    const int maxSprites = 120000;
    int numberOfSprites = 10;
    std::vector<Sprite*> *sprites;
public:

    // inheriting constructor
    using Screen::Screen;

    void init() override;
    void update(float dt) override;
    void render() override;
    void resize(int width, int height) override;

};


#endif //TOWERDEFENSE_TESTSCREEN_H
