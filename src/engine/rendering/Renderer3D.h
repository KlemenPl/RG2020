//
// Created by klemen on 18/12/2020.
//

#ifndef TOWERDEFENSE_RENDERER3D_H
#define TOWERDEFENSE_RENDERER3D_H

#include <unordered_map>
#include <vector>
#include "../graphics/Model.h"
#include "../camera/Camera.h"

class Renderer3D
{
private:
    std::unordered_map<uint32_t, std::vector<Model>> instancedQueue;
    std::unordered_map<uint32_t, std::vector<Model>> shadowQueue;
    std::vector<int> lights;
    Camera *camera;

    std::vector<glm::mat4> instanedModels;
    uint32_t instanceBuffer;
    uint32_t instanceBufferLength;

    const uint32_t lightLimit = 4;


    Ref<Shader> materialShader;
    Ref<Shader> texturedShader;
    Ref<Shader> shadowShader;
    Ref<Shader> lightShader;
public:
    Renderer3D();
    virtual ~Renderer3D();

    void begin();
    void draw(const Model& model);
    void end();

    void flush();

    void beginShadows(int* light);
    void drawShadow(const Model& model);
    void endShadows();

    void setCamera(Camera* camera);
    void addLight(int* light);
    void removeLight(int* light);
    void clearLights();

    // should not be copied!!
    Renderer3D(const Renderer3D &) = delete;
    Renderer3D &operator=(const Renderer3D &) = delete;
};


#endif //TOWERDEFENSE_RENDERER3D_H
