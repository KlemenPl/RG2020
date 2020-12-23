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
    Material material;


    Ref<RawModel> bunny;
    Ref<RawModel> cube;

    Ref<BitmapFont> font;
    Ref<Shader> shader;
    Ref<Shader> lightShader;

    float deltaTime{};
    float lastFrame{};

    const int maxBunnies = 1000;
    int numberOfBunnies = 80;

    glm::mat4* modelMatrices;
    uint32_t instanceBuffer;

};


#endif //TOWERDEFENSE_INSTANCINGTEST_H
