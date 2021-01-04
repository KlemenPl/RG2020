//
// Created by klemen on 18/12/2020.
//

#ifndef TOWERDEFENSE_RENDERER3D_H
#define TOWERDEFENSE_RENDERER3D_H

#include <unordered_map>
#include <vector>
#include <set>
#include "../graphics/Model.h"
#include "../graphics/Light.h"
#include "../camera/PerspectiveCamera.h"
#include "../graphics/Terrain.h"
#include "../graphics/FrameBuffer.h"
#include "../graphics/CubeMap.h"

class Renderer3D
{
private:
    const uint32_t INSTANCE_LIMIT = 2000;
    const uint32_t MATERIAL_LIMIT = 20;
    const uint32_t DIR_LIGHT_LIMIT = 2;
    const uint32_t POINT_LIGHT_LIMIT = 5;

    std::unordered_map<uint32_t, std::vector<Model *>> instancedQueue;
    std::unordered_map<uint32_t, std::vector<Model *>> shadowQueue;

    std::vector<DirLight> dirLights;
    std::vector<PointLight> pointLights;

    PerspectiveCamera *camera = nullptr;
    bool drawing = false;
    bool drawingShadows = false;

    uint32_t drawCalls = 0;
    uint32_t currentCullStrategy = GL_NONE;

    std::set<uint32_t> preparedModels;

    std::vector<glm::mat4> modelMatrices;
    uint32_t modelMatricesSSBO{};
    uint32_t modelMatricesSSBOLength{};

    uint32_t vpMatricesUBO{};

    uint32_t lightsUBO{};
    uint32_t lightsBufferSize{};
    float *lightsBufferUBO{};

    float *materialBuffer;

    //std::vector<Material> materials;
    uint32_t materialsSSBO{};
    uint32_t materialsSSBOLength{};

    Ref<Shader> shader;
    Ref<Shader> terrainShader;
    Ref<Shader> skyboxShader;
    Ref<Shader> waterShader;
    Ref<Shader> shadowShader;
    Ref<Shader> normalDebugShader;
    Ref<Shader> lightShader;

    FrameBuffer reflectionFB;
    FrameBuffer refractionFB;

    CubeMap *skybox = nullptr;
    Terrain *terrain = nullptr;

    void prepareRawModel(const RawModel &model);

    void setupCamera();

    void drawTerrain();
    void drawSkybox();

public:
    Renderer3D();
    virtual ~Renderer3D();

    void drawTerrain(const Terrain& terrain);


    void begin();
    void draw(Model *model);
    void end();
    void flush(const Group &group);

    void drawNormals(Model *model);

    void beginShadows(DirLight *dirLight);
    void drawShadow(Model *model);
    void endShadows();
    void flushShadows();

    void setCamera(PerspectiveCamera *_camera);

    void addPointLight(PointLight light);
    void removePointLight(const PointLight &light);
    void removePointLight(uint32_t lightIndex);
    void clearPointLights();

    void addDirLight(DirLight light);
    void removeDirLight(const DirLight &light);
    void removeDirLight(uint32_t lightIndex);
    void clearDirLights();

    void setTerrain(Terrain *_terrain);
    void setSkybox(CubeMap *_skybox);

    // should not be copied!!
    Renderer3D(const Renderer3D &) = delete;
    Renderer3D &operator=(const Renderer3D &) = delete;

    const FrameBuffer &getReflectionFb() const;
    const FrameBuffer &getRefractionFb() const;
};


#endif //TOWERDEFENSE_RENDERER3D_H
