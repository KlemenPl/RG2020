//
// Created by klemen on 29/12/2020.
//

#ifndef TOWERDEFENSE_TERRAINTEST_H
#define TOWERDEFENSE_TERRAINTEST_H


#include "Test.h"
#include "../engine/camera/PerspectiveCamera.h"
#include "../engine/camera/MouseCameraController.h"
#include "../engine/rendering/Renderer3D.h"
#include "../engine/graphics/Terrain.h"

class TerrainTest : public Test
{
public:
    void init() override;
    void start() override;

private:
    PerspectiveCamera *camera;
    MouseCameraController* mcc;
    Renderer3D *renderer3D;
    Terrain *terrain;

    float deltaTime{};
    float lastFrame{};

};


#endif //TOWERDEFENSE_TERRAINTEST_H
