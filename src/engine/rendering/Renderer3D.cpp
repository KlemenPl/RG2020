//
// Created by klemen on 18/12/2020.
//

#include <stdexcept>
#include <ext.hpp>
#include <algorithm>
#include <utility>
#include "Renderer3D.h"
#include "../ResourceManager.h"
#include "../../game/Game.h"
#include "../utils/Hash.h"

Renderer3D::Renderer3D() :
        reflectionFB(FrameBuffer(Game::width / 4, Game::height / 4)),
        refractionFB(FrameBuffer(Game::width, Game::height))
//,shadowFB(FrameBuffer(Game::width / 2, Game::height / 2))
{
    ResourceManager::loadShader("res/shaders/default3D_VS.glsl",
                                "res/shaders/default3D_FS.glsl",
                                nullptr, "default3D");
    defaultShader = ResourceManager::getShader("default3D");
    ResourceManager::loadShader("res/shaders/wavey_VS.glsl",
                                "res/shaders/default3D_FS.glsl",
                                nullptr, "wavey");
    waveyShader = ResourceManager::getShader("wavey");

    //shadowShader = ResourceManager::getShader("shadowShader");
    lightShader = ResourceManager::getShader("LightShader");

    ResourceManager::loadShader("res/shaders/debug_normal_VS.glsl",
                                "res/shaders/debug_normal_FS.glsl",
                                "res/shaders/debug_normal_GS.glsl", "normalDebug");
    normalDebugShader = ResourceManager::getShader("normalDebug");

    ResourceManager::loadShader("res/shaders/terrain_VS.glsl",
                                "res/shaders/terrain_FS.glsl",
                                nullptr, "terrain");
    terrainShader = ResourceManager::getShader("terrain");

    ResourceManager::loadShader("res/shaders/water_VS.glsl",
                                "res/shaders/water_FS.glsl",
                                nullptr, "water");
    waterShader = ResourceManager::getShader("water");

    ResourceManager::loadShader("res/shaders/skybox_VS.glsl",
                                "res/shaders/skybox_FS.glsl",
                                nullptr, "skybox"
    );
    skyboxShader = ResourceManager::getShader("skybox");
    skyboxShader->bind();
    skyboxShader->setUniform("skybox", 0);

    waterShader->bind();
    waterShader->setUniform("reflectionSample", 0);
    waterShader->setUniform("refractionSample", 1);
    waterShader->setUniform("dudvSample", 2);
    waterShader->setUniform("normalSample", 3);
    waterShader->setUniform("depthSample", 4);

    reflectionFB.createColourAttachment();
    refractionFB.createDepthAndColourAttachment();
    //shadowFB.createDepthAttachment();

    // generating SSBOs
    glGenBuffers(1, &modelMatricesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatricesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, INSTANCE_LIMIT * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &materialsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, MATERIAL_LIMIT * sizeof(Material), nullptr, GL_DYNAMIC_DRAW);
    materialBuffer = new float[MATERIAL_LIMIT * Material::GLSL_SIZE];
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // generating UBO for vpMatrices
    glGenBuffers(1, &vpMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, vpMatricesUBO);
    // 3 matrices (view, projection and combined)
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

    // generating UBO for lights
    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    lightsBufferSize = DIR_LIGHT_LIMIT * DirLight::GLSL_BYTE_SIZE + POINT_LIGHT_LIMIT * PointLight::GLSL_BYTE_SIZE +
                       1 * sizeof(glm::vec4) + 4 * sizeof(float);
    lightsBufferUBO = new float[lightsBufferSize];
    glBufferData(GL_UNIFORM_BUFFER, lightsBufferSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0); // unbinding

}
Renderer3D::~Renderer3D()
{
    glDeleteBuffers(1, &modelMatricesSSBO);
    glDeleteBuffers(1, &materialsSSBO);
    glDeleteBuffers(1, &vpMatricesUBO);

    delete[] materialBuffer;
    delete[] lightsBufferUBO;

    modelMatrices.clear();

}

void Renderer3D::reset()
{
    clearDirLights();
    clearPointLights();
    clearQueues();

    camera = nullptr;
    terrain = nullptr;
    skybox = nullptr;

    drawing = false;
    drawCalls = 0;
    currentCullStrategy = GL_NONE;
    preparedModels.clear();
    modelMatrices.clear();

}

void Renderer3D::prepareRawModel(const RawModel &model)
{
    if (preparedModels.find(model.modelID) == preparedModels.end())
    {
        // preparing instance matrix
        for (auto &it:model.groups)
        {
            it.mesh.bind();
            glBindBuffer(GL_ARRAY_BUFFER, modelMatricesSSBO);

            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) 0);

            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (sizeof(glm::vec4)));

            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (2 * sizeof(glm::vec4)));

            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(6, 1);
            glVertexAttribDivisor(7, 1);
            glVertexAttribDivisor(8, 1);
            glVertexAttribDivisor(9, 1);
        }

        preparedModels.insert(model.modelID);
    }
}

void Renderer3D::clearQueue(Renderer3D::ModelQueue &queue)
{
    for (auto &modelsByShader:queue)
    {
        for (auto &models:modelsByShader.second)
            models.second.clear();
        modelsByShader.second.clear();
    }
    queue.clear();
}
void Renderer3D::clearQueues()
{
    clearQueue(modelsQueue);
    clearQueue(reflectionQueue);
    //clearQueue(shadowQueue);
}

void Renderer3D::drawTerrain()
{
    if (terrain == nullptr)
        return;

    for (const auto &tree : terrain->solid)
    {
        draw((Model *) &tree, true, true);
    }
    for (const auto &shrub : terrain->shrubs)
    {
        draw((Model *) &shrub, true, true,
             waveyShader._get_ptr(),
             [](Shader *s) {
                 s->setUniform("time", (float) glfwGetTime());
             });
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3 camPos = camera->getPosition();
    float distance = 2 * (camera->getPosition().y - terrain->waterLevel);
    camPos.y -= distance;
    camera->setPosition(camPos);
    camera->setPitch(-camera->getPitch());
    camera->update();
    setupCamera();
    // reflection
    reflectionFB.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawModels(reflectionQueue);
    terrain->terrainMesh.bind();
    terrainShader->bind();
    terrainShader->setUniform("plane", glm::vec4(0, 1, 0, -terrain->waterLevel));
    glDrawElements(GL_TRIANGLES, terrain->terrainMesh.indicesLength, GL_UNSIGNED_INT, (void *) 0);

    camPos.y += distance;
    camera->setPosition(camPos);
    camera->setPitch(-camera->getPitch());
    camera->update();
    setupCamera();
    drawSkybox();
    terrain->terrainMesh.bind();
    terrainShader->bind();
    reflectionFB.unbind();
    // refraction
    refractionFB.bind();
    terrainShader->setUniform("plane", glm::vec4(0, -1, 0, terrain->waterLevel));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, terrain->terrainMesh.indicesLength, GL_UNSIGNED_INT, (void *) 0);

    refractionFB.unbind();

    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    terrainShader->setUniform("plane", glm::vec4(0, -1, 0, 1000.0f));

    glDrawElements(GL_TRIANGLES, terrain->terrainMesh.indicesLength, GL_UNSIGNED_INT, (void *) 0);

    waterShader->bind();
    terrain->waterMesh.bind();
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, reflectionFB.colourAttachment->ID);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, refractionFB.colourAttachment->ID);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, terrain->waterDuDvMap->ID);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, terrain->waterNormalMap->ID);
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, refractionFB.depthAttachment->ID);

    waterShader->setUniform("moveFactor", (float) glfwGetTime() / 40.0f);
    waterShader->setUniform("camPos", camPos);

    glDrawElements(GL_TRIANGLES, terrain->waterMesh.indicesLength, GL_UNSIGNED_INT, (void *) 0);


    glDisable(GL_BLEND);

    drawSkybox();
}
void Renderer3D::drawSkybox()
{
    if (!skybox)
        return;

    glDepthFunc(GL_LEQUAL);
    //glDepthMask(GL_FALSE);

    skybox->bind();
    skyboxShader->bind();
    skyboxShader->setUniform("projection", camera->getProjectionMatrix());
    skyboxShader->setUniform("view",
                             glm::mat4(glm::mat3(camera->getViewMatrix())));
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *) 0);
    skybox->unbind();

    //glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}


void Renderer3D::begin()
{
    if (drawing)
        throw std::runtime_error("Renderer3D::begin(): Cannot begin while it is already drawing!");
    if (!camera)
        throw std::runtime_error("Renderer3D::begin(): Camera not set!");

    drawing = true;
    drawCalls = 0;

    glEnable(GL_CULL_FACE);
    glEnable(GL_CLIP_DISTANCE0);
    glEnable(GL_DEPTH_TEST);
    //glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer3D::draw(Model *model, bool reflection, bool shadows)
{
    draw(model, reflection, shadows, defaultShader._get_ptr());
}

void Renderer3D::draw(Model *model, bool reflection, bool shadows, Shader *useShader,
                      const std::function<void(Shader *)> &shaderUniforms)
{
    insertIntoQueue(modelsQueue, model, useShader, shaderUniforms);

    if (reflection)
        insertIntoQueue(reflectionQueue, model, useShader, shaderUniforms);
    //if (shadows)
    //    insertIntoQueue(shadowQueue, model, useShader, shaderUniforms);
}

void Renderer3D::insertIntoQueue(ModelQueue &queue, Model *model,
                                 Shader *useShader, std::function<void(Shader *)> shaderUniforms)
{
    size_t seed = 0;
    Utils::hash_combine<uint32_t>(seed, model->rawModel.modelID);
    for (auto &group:model->modelGroups)
        if (!group.enabled)
            Utils::hash_combine<uint32_t>(seed, group.mesh->VAO);
    ShaderSetup shaderSetup{useShader, std::move(shaderUniforms)};
    queue[shaderSetup][seed].push_back(model);
}
void Renderer3D::drawModelsGroup(const Group &group)
{
    glBindVertexArray(group.mesh.VAO);

    // writing model matrices
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatricesSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, modelMatrices.size() * sizeof(glm::mat4),
                    &modelMatrices[0][0][0]);

    // writing materials
    uint32_t mIdx = 0;

    // https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
    auto &materials = group.meshMaterials;
    for (auto &m:materials)
    {
        //@formatter:off
        materialBuffer[mIdx + 0]  = m.Ka.x;
        materialBuffer[mIdx + 1]  = m.Ka.y;
        materialBuffer[mIdx + 2]  = m.Ka.z;
        // 3
        materialBuffer[mIdx + 4]  = m.Kd.x;
        materialBuffer[mIdx + 5]  = m.Kd.y;
        materialBuffer[mIdx + 6]  = m.Kd.z;
        // 7
        materialBuffer[mIdx + 8]  = m.Ks.x;
        materialBuffer[mIdx + 9]  = m.Ks.y;
        materialBuffer[mIdx + 10] = m.Ks.z;
        // 11
        materialBuffer[mIdx + 12] = m.Ns;
        materialBuffer[mIdx + 14] = m.Ni;
        //@formatter:on
        mIdx += Material::GLSL_SIZE;
    }

    if (mIdx > 0)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, mIdx * sizeof(float), materialBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, materialsSSBO);
    }

    // drawing
    //rawModel.groups[i].mesh.bind();
    //glDrawElements(GL_TRIANGLES,rawModel.groups[i].mesh.indicesLength,GL_UNSIGNED_INT, nullptr);
    glDrawElementsInstanced(GL_TRIANGLES, group.mesh.indicesLength, GL_UNSIGNED_INT, nullptr,
                            modelMatrices.size());

    modelMatrices.clear();
}
void Renderer3D::drawModels(ModelQueue &queue)
{
    for (auto &byShader:queue)
    {
        Shader *shader = byShader.first.shaderProgram;
        shader->bind();
        byShader.first.setup(shader);

        // instancing meshes
        for (auto &models : byShader.second)
        {
            if (models.second.empty())
                continue;


            auto &modelVector = models.second;
            //prepareRawModel(modelVector.front()->rawModel);

            auto &model = modelVector.front();
            auto &rawModel = modelVector.front()->rawModel;

            if (currentCullStrategy != rawModel.cullStrategy)
            {
                glFrontFace(rawModel.cullStrategy);
                currentCullStrategy = rawModel.cullStrategy;
            }

            uint32_t groupSize = modelVector.front()->modelGroups.size();
            uint32_t modelLength = models.second.size();

            for (int i = 0; i < groupSize; i++)
            {

                if (!model->modelGroups[i].enabled)
                    continue;

                for (int j = 0; j < modelLength; j++)
                {
                    if (j > 0 && j % INSTANCE_LIMIT == 0)
                        drawModelsGroup(rawModel.groups[i]);

                    // transforming
                    auto &modelGroup = modelVector[j]->modelGroups[i];
                    modelMatrices.push_back(modelGroup.modelMatrix);
                }
                drawModelsGroup(rawModel.groups[i]);
            }
        }
    }
}

void Renderer3D::setupCamera()
{
    // seting vpMatrices UBO
    glBindBuffer(GL_UNIFORM_BUFFER, vpMatricesUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, vpMatricesUBO);
    // should be fine, since p,v, combined are in sequential order
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 3 * sizeof(glm::mat4), &camera->getProjectionMatrix()[0][0]);
}

void Renderer3D::transformMatrices()
{
    for (auto &byShader:modelsQueue)
    {
        // iterating over meshes
        for (auto &models : byShader.second)
        {
            if (models.second.empty())
                continue;

            auto &modelVector = models.second;
            prepareRawModel(modelVector.front()->rawModel);

            auto &model = modelVector.front();
            uint32_t groupSize = modelVector.front()->modelGroups.size();
            uint32_t modelLength = models.second.size();

            for (int i = 0; i < groupSize; i++)
            {
                if (!model->modelGroups[i].enabled)
                    continue;
                for (int j = 0; j < modelLength; j++)
                {
                    // transforming
                    auto &modelGroup = modelVector[j]->modelGroups[i];

                    if (!modelGroup.isStatic)
                        modelGroup.transform();
                }
            }
        }
    }
}

void Renderer3D::end()
{
    if (!drawing)
        throw std::runtime_error("Renderer3D::end(): Renderer must first be started");

    setupCamera();
    defaultShader->bind();
    defaultShader->setUniform("time", (float) glfwGetTime());

    // sorting point lights
    //std::sort(pointLights.begin(), pointLights.end(), DistanceFromCamera::compareDistance);
    std::sort(pointLights.begin(), pointLights.end(),
              [this](const PointLight &left, const PointLight &right) {
                  return (glm::distance(camera->getPosition(), left.position)
                          < glm::distance(camera->getPosition(), right.position));
              });

    // applying dir lights
    uint32_t length = dirLights.size();
    if (length > DIR_LIGHT_LIMIT)
        length = DIR_LIGHT_LIMIT;

    float dirLightLength = *(float *) &length;

    uint32_t lIdx = 0;
    for (int i = 0; i < length; i++)
    {
        lightsBufferUBO[lIdx + 0] = dirLights[i].direction.x;
        lightsBufferUBO[lIdx + 1] = dirLights[i].direction.y;
        lightsBufferUBO[lIdx + 2] = dirLights[i].direction.z;
        // padding
        lightsBufferUBO[lIdx + 4] = dirLights[i].Ka.x;
        lightsBufferUBO[lIdx + 5] = dirLights[i].Ka.y;
        lightsBufferUBO[lIdx + 6] = dirLights[i].Ka.z;
        // padding
        lightsBufferUBO[lIdx + 8] = dirLights[i].Kd.x;
        lightsBufferUBO[lIdx + 9] = dirLights[i].Kd.y;
        lightsBufferUBO[lIdx + 10] = dirLights[i].Kd.z;
        // padding
        lightsBufferUBO[lIdx + 12] = dirLights[i].Ks.x;
        lightsBufferUBO[lIdx + 13] = dirLights[i].Ks.y;
        lightsBufferUBO[lIdx + 14] = dirLights[i].Ks.z;
        // padding

        lIdx += DirLight::GLSL_SIZE;
    }
    lIdx = DirLight::GLSL_SIZE * DIR_LIGHT_LIMIT;

    // applying point lights
    length = pointLights.size();
    if (length > POINT_LIGHT_LIMIT)
        length = POINT_LIGHT_LIMIT;

    float pointLightLength = *(float *) &length;

    for (int i = 0; i < length; i++)
    {
        lightsBufferUBO[lIdx + 0] = pointLights[i].position.x;
        lightsBufferUBO[lIdx + 1] = pointLights[i].position.y;
        lightsBufferUBO[lIdx + 2] = pointLights[i].position.z;
        // padding
        lightsBufferUBO[lIdx + 4] = pointLights[i].Ka.x;
        lightsBufferUBO[lIdx + 5] = pointLights[i].Ka.y;
        lightsBufferUBO[lIdx + 6] = pointLights[i].Ka.z;
        // padding
        lightsBufferUBO[lIdx + 8] = pointLights[i].Kd.x;
        lightsBufferUBO[lIdx + 9] = pointLights[i].Kd.y;
        lightsBufferUBO[lIdx + 10] = pointLights[i].Kd.z;
        // padding
        lightsBufferUBO[lIdx + 12] = pointLights[i].Ks.x;
        lightsBufferUBO[lIdx + 13] = pointLights[i].Ks.y;
        lightsBufferUBO[lIdx + 14] = pointLights[i].Ks.z;
        // padding
        lightsBufferUBO[lIdx + 16] = pointLights[i].constant;
        lightsBufferUBO[lIdx + 17] = pointLights[i].linear;
        lightsBufferUBO[lIdx + 18] = pointLights[i].quadratic;
        // padding

        lIdx += PointLight::GLSL_SIZE;
    }

    lIdx = DirLight::GLSL_SIZE * DIR_LIGHT_LIMIT +
           PointLight::GLSL_SIZE * POINT_LIGHT_LIMIT;

    auto &viewPos = camera->getPosition();
    lightsBufferUBO[lIdx + 0] = viewPos.x;
    lightsBufferUBO[lIdx + 1] = viewPos.y;
    lightsBufferUBO[lIdx + 2] = viewPos.z;

    lightsBufferUBO[lIdx + 4] = dirLightLength;
    lightsBufferUBO[lIdx + 5] = pointLightLength;


    // seting vpMatrices UBO
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, lightsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, lightsBufferSize, lightsBufferUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    drawTerrain();
    transformMatrices();
    //shadowFB.bind();
    //glClear(GL_DEPTH_BUFFER_BIT);
    //drawModels(shadowQueue);
    //shadowFB.unbind();
    drawModels(modelsQueue);
    clearQueues();

    drawing = false;
}

void Renderer3D::drawUninstanced(Model *model)
{
    for (auto &group:model->modelGroups)
    {
        if (!group.enabled)
            continue;

        if (!group.isStatic)
            group.transform();

        group.mesh->bind();
        glDrawElements(GL_TRIANGLES, group.mesh->indicesLength, GL_UNSIGNED_INT, (void *) 0);
    }
}

void Renderer3D::drawUninstanced(Model *model, Shader *useShader, const std::function<void(Shader *)> &shaderUniforms)
{
    useShader->bind();
    shaderUniforms(useShader);
    drawUninstanced(model);
}

void Renderer3D::setTerrain(Terrain *_terrain)
{
    this->terrain = _terrain;
}
void Renderer3D::setSkybox(CubeMap *_skybox)
{
    this->skybox = _skybox;
}

void Renderer3D::drawNormals(Model *model)
{
    const RawModel &rawModel = model->rawModel;
    normalDebugShader->bind();

    for (int i = 0; i < rawModel.groups.size(); i++)
    {
        rawModel.groups[i].mesh.bind();
        auto &modelGroup = model->modelGroups[i];
        if (!modelGroup.isStatic)
            modelGroup.transform();
        normalDebugShader->setUniform("model", modelGroup.modelMatrix);

        glDrawElements(GL_TRIANGLES, rawModel.groups[i].mesh.indicesLength, GL_UNSIGNED_INT, nullptr);
    }

}

void Renderer3D::setCamera(PerspectiveCamera *_camera)
{
    this->camera = _camera;
}

void Renderer3D::addPointLight(PointLight light)
{
    // copy is fine
    pointLights.emplace_back(light);

}
void Renderer3D::removePointLight(const PointLight &light)
{
    for (int i = 0; i < pointLights.size(); i++)
    {
        if (pointLights[i] == light)
        {
            pointLights.erase(pointLights.begin() + i);
            break;
        }
    }
}
void Renderer3D::removePointLight(uint32_t lightIndex)
{
    pointLights.erase(pointLights.begin() + lightIndex);
}
void Renderer3D::clearPointLights()
{
    pointLights.clear();
}

void Renderer3D::addDirLight(DirLight light)
{
    // copy is fine
    dirLights.emplace_back(light);
}
void Renderer3D::removeDirLight(const DirLight &light)
{
    for (int i = 0; i < dirLights.size(); i++)
    {
        if (dirLights[i] == light)
        {
            dirLights.erase(dirLights.begin() + i);
            break;
        }
    }
}
void Renderer3D::removeDirLight(uint32_t lightIndex)
{
    dirLights.erase(dirLights.begin() + lightIndex);
}
void Renderer3D::clearDirLights()
{
    dirLights.clear();
}
const FrameBuffer &Renderer3D::getReflectionFb() const
{
    return reflectionFB;
}
const FrameBuffer &Renderer3D::getRefractionFb() const
{
    return refractionFB;
}
