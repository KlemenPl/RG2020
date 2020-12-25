//
// Created by klemen on 23/12/2020.
//

#ifndef TOWERDEFENSE_INSTANCING2TEST_H
#define TOWERDEFENSE_INSTANCING2TEST_H

#include "Test.h"
#include "../engine/Core.h"
#include "../engine/camera/PerspectiveCamera.h"
#include "../engine/camera/MouseCameraController.h"
#include "../engine/graphics/RawModel.h"
#include "../engine/rendering/Renderer3D.h"

class Instancing2Test: public Test
{
public:
    void init() override;
    void start() override;
private:
    PerspectiveCamera *camera;
    MouseCameraController* cameraController;
    Renderer3D* renderer3D;

    Ref<RawModel> tree;

    Ref<BitmapFont> font;

    float deltaTime{};
    float lastFrame{};
};


#endif //TOWERDEFENSE_INSTANCING2TEST_H
