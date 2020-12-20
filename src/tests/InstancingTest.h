//
// Created by klemen on 18/12/2020.
//

#ifndef TOWERDEFENSE_INSTANCINGTEST_H
#define TOWERDEFENSE_INSTANCINGTEST_H

#include "Test.h"
#include "../engine/Core.h"
#include "../engine/camera/PerspectiveCamera.h"
#include "../engine/camera/MouseCameraController.h"
#include "../engine/graphics/RawModel.h"

class InstancingTest : public Test
{
public:
    void init() override;
    void start() override;
private:
    PerspectiveCamera *camera;
    MouseCameraController* cameraController;


    glm::vec3 lightPos;
    Material lightMaterial;
    Ref<RawModel> cube;

    Material material;


    Ref<RawModel> bunny;
    Ref<BitmapFont> font;
    Ref<Shader> shader;

    float deltaTime{};
    float lastFrame{};

    const int maxBunnies = 200000;
    int numberOfBunnies;

};


#endif //TOWERDEFENSE_INSTANCINGTEST_H
