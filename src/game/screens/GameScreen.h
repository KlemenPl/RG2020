//
// Created by klemen on 27/12/2020.
//

#ifndef TOWERDEFENSE_GAMESCREEN_H
#define TOWERDEFENSE_GAMESCREEN_H


#include "Screen.h"
#include "../../engine/camera/PerspectiveCamera.h"
#include "../../engine/camera/OrthographicCamera.h"
#include "../../engine/graphics/Terrain.h"

class GameScreen : public Screen
{
private:
    PerspectiveCamera *camera;
    OrthographicCamera *orthoCamera;
    Terrain* terrain;

public:
    using Screen::Screen;

    virtual ~GameScreen() override;

    void init() override;
    void update(float dt) override;
    void render() override;
    void resize(int width, int height) override;

};


#endif //TOWERDEFENSE_GAMESCREEN_H
