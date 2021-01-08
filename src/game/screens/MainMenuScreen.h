//
// Created by klemen on 06/01/2021.
//

#ifndef TOWERDEFENSE_MAINMENUSCREEN_H
#define TOWERDEFENSE_MAINMENUSCREEN_H

#include "Screen.h"
#include "../../engine/camera/OrthographicCamera.h"
#include "../../engine/camera/PerspectiveCamera.h"
#include "../../engine/graphics/Terrain.h"
#include "../ui/GUI.h"

class MainMenuScreen : public Screen
{
private:
    PerspectiveCamera *camera;
    OrthographicCamera *orthoCamera;
    Terrain *terrain;
    GUI *gui;

public:
    using Screen::Screen;

    virtual ~MainMenuScreen() override;

    void init() override;
    void update(float dt) override;
    void render() override;
    void resize(int width, int height) override;
    void show() override;

};


#endif //TOWERDEFENSE_MAINMENUSCREEN_H
