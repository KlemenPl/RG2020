//
// Created by klemen on 18/12/2020.
//

#ifndef TOWERDEFENSE_RENDERER3D_H
#define TOWERDEFENSE_RENDERER3D_H

#include <unordered_map>
#include <vector>
#include <set>
#include <functional>
#include "../graphics/Model.h"
#include "../graphics/Light.h"
#include "../graphics/Shader.h"
#include "../camera/PerspectiveCamera.h"
#include "../graphics/Terrain.h"
#include "../graphics/FrameBuffer.h"
#include "../graphics/CubeMap.h"

struct ShaderSetup
{
    Shader *shaderProgram;
    std::function<void(Shader *)> setup = [](Shader *) {};

    bool operator==(const ShaderSetup &rhs) const
    {
        return shaderProgram == rhs.shaderProgram;
    }
    bool operator!=(const ShaderSetup &rhs) const
    {
        return !(rhs == *this);
    }

};

namespace std {

    template<>
    struct hash<ShaderSetup>
    {

        std::size_t operator()(const ShaderSetup &k) const
        {
            using std::hash;
            return hash<Shader *>()(k.shaderProgram);
        }
    };

}

class Renderer3D
{
private:
    const uint32_t INSTANCE_LIMIT = 2000;
    const uint32_t MATERIAL_LIMIT = 20;
    const uint32_t DIR_LIGHT_LIMIT = 2;
    const uint32_t POINT_LIGHT_LIMIT = 5;

    using ModelQueue = std::unordered_map<ShaderSetup,
            std::unordered_map<uint64_t, std::vector<Model *>>>;

    ModelQueue modelsQueue;
    ModelQueue reflectionQueue;
    //ModelQueue shadowQueue;

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

    Ref<Shader> defaultShader;
    Ref<Shader> waveyShader;
    Ref<Shader> terrainShader;
    Ref<Shader> skyboxShader;
    Ref<Shader> waterShader;
    //Ref<Shader> shadowShader;
    Ref<Shader> normalDebugShader;
    Ref<Shader> lightShader;

    FrameBuffer reflectionFB;
    FrameBuffer refractionFB;
    //FrameBuffer shadowFB;

    CubeMap *skybox = nullptr;
    Terrain *terrain = nullptr;

    void prepareRawModel(const RawModel &model);

    void setupCamera();

    void drawTerrain();
    void drawSkybox();

    void clearQueue(ModelQueue& queue);
    void clearQueues();

    void transformMatrices();

    void drawModelsGroup(const Group &group);
    void drawModels(ModelQueue &queue);

    void insertIntoQueue(ModelQueue &queue,
                         Model *model, Shader *useShader,
                         std::function<void(Shader *)> shaderUniforms = [](Shader *) {});

public:
    Renderer3D();
    virtual ~Renderer3D();

    void begin();
    void draw(Model *model, bool reflection = false, bool shadows = false);
    void draw(Model *model, bool reflection, bool shadows,
              Shader *useShader, const std::function<void(Shader *)>& shaderUniforms = [](Shader *) {});
    void end();

    void drawNormals(Model *model);

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

    void reset();

    // should not be copied!!
    Renderer3D(const Renderer3D &) = delete;
    Renderer3D &operator=(const Renderer3D &) = delete;

    const FrameBuffer &getReflectionFb() const;
    const FrameBuffer &getRefractionFb() const;
};


#endif //TOWERDEFENSE_RENDERER3D_H
