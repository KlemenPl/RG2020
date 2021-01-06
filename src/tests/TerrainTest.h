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
#include "../engine/rendering/Renderer2D.h"
#include "../engine/camera/OrthographicCamera.h"

class TerrainTest : public Test
{
public:
    void init() override;
    void start() override;

private:
    PerspectiveCamera *camera;
    OrthographicCamera *orthoCamera;
    MouseCameraController* mcc;
    Renderer2D *renderer2D;
    Renderer3D *renderer3D;
    Terrain *terrain;
    Ref<CubeMap> skybox;
    Ref<Shader> debugDepthShader;

    float deltaTime{};
    float lastFrame{};

};


#endif //TOWERDEFENSE_TERRAINTEST_H
