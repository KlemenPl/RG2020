//
// Created by klemen on 18/12/2020.
//

#include <stdexcept>
#include <ext.hpp>
#include <algorithm>
#include "Renderer3D.h"
#include "../ResourceManager.h"

Renderer3D::Renderer3D()
{
    ShaderSourceArgument args[2];
    args[0] = {VERTEX, "MAX_DIR_LIGHTS", std::to_string(DIR_LIGHT_LIMIT)};
    args[1] = {VERTEX, "MAX_POINT_LIGHTS", std::to_string(POINT_LIGHT_LIMIT)};
    ResourceManager::loadShader("res/shaders/default3D_VS.glsl",
                                "res/shaders/default3D_FS.glsl",
                                nullptr, "default3D", args, 2);
    shader = ResourceManager::getShader("default3D");
    shadowShader = ResourceManager::getShader("shadowShader");
    lightShader = ResourceManager::getShader("LightShader");

    ResourceManager::loadShader("res/shaders/debug_normal_VS.glsl",
                                "res/shaders/debug_normal_FS.glsl",
                                "res/shaders/debug_normal_GS.glsl", "normalDebug");
    normalDebugShader = ResourceManager::getShader("normalDebug");

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
    lightsBufferSize = DIR_LIGHT_LIMIT * DirLight::GLSL_SIZE + POINT_LIGHT_LIMIT * PointLight::GLSL_SIZE +
                       2 * sizeof(int) + sizeof(glm::vec4);
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
void Renderer3D::begin()
{
    if (drawing)
        throw std::runtime_error("Renderer3D::begin(): Cannot begin while it is already drawing!");
    if (!camera)
        throw std::runtime_error("Renderer3D::begin(): Camera not set!");

    drawing = true;
    drawCalls = 0;

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void Renderer3D::draw(Model *model)
{
    uint32_t id = model->rawModel.modelID;
    instancedQueue[id].push_back(model);
}
void Renderer3D::end()
{
    if (!drawing)
        throw std::runtime_error("Renderer3D::end(): Renderer must first be started");

    // seting vpMatrices UBO
    glBindBuffer(GL_UNIFORM_BUFFER, vpMatricesUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, vpMatricesUBO);
    // should be fine, since p,v, combined are in sequential order
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 3 * sizeof(glm::mat4), &camera->getProjectionMatrix()[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shader->bind();

    // sorting point lights
    //std::sort(pointLights.begin(), pointLights.end(), DistanceFromCamera::compareDistance);
    std::sort(pointLights.begin(), pointLights.end(),
              [this](const PointLight &left, const PointLight &right) {
                  return (glm::distance(camera->getPosition(), left.position)
                          < glm::distance(camera->getPosition(), right.position));
              });

    // applying point lights
    int length = pointLights.size();
    if (length > POINT_LIGHT_LIMIT)
        length = POINT_LIGHT_LIMIT;
    shader->setUniform("NUM_POINT_LIGHTS", length);

    for (int i = 0; i < length; i++)
    {
        shader->setUniformPointLight("pointLights[" + std::to_string(i) + "]", pointLights[i]);
    }

    // applying dir lights
    length = dirLights.size();
    if (length > DIR_LIGHT_LIMIT)
        length = DIR_LIGHT_LIMIT;
    shader->setUniform("NUM_DIR_LIGHTS", length);

    for (int i = 0; i < length; i++)
    {
        shader->setUniformDirLight("dirLights[" + std::to_string(i) + "]", dirLights[i]);
    }

    shader->setUniform("combined", camera->getCombined());
    shader->setUniform("viewPos", camera->getPosition());

    // instancing meshes
    for (auto &models : instancedQueue)
    {
        if (models.second.empty())
            continue;


        auto &modelVector = models.second;
        prepareRawModel(modelVector.front()->rawModel);

        auto &model = modelVector.front();
        auto &rawModel = modelVector.front()->rawModel;

        if (currentCullStrategy != rawModel.cullStrategy)
        {
            glFrontFace(rawModel.cullStrategy);
            currentCullStrategy = rawModel.cullStrategy;
        }

        uint32_t groupSize = modelVector.front()->modelGroups.size();

        for (int i = 0; i < groupSize; i++)
        {
            if(i%INSTANCE_LIMIT==0)
                flush();
            uint32_t modelLength = models.second.size();

            modelMatrices.clear();

            for (int j = 0; j < modelLength; j++)
            {
                // transforming
                auto &modelGroup = modelVector[j]->modelGroups[i];

                if (!modelGroup.isStatic)
                    modelGroup.transform();
                modelMatrices.push_back(modelGroup.modelMatrix);
            }

            glBindVertexArray(rawModel.groups[i].mesh.VAO);

            // writing model matrices
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatricesSSBO);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, modelMatrices.size() * sizeof(glm::mat4),
                            &modelMatrices[0][0][0]);

            // writing materials
            uint32_t mIdx = 0;

            // https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
            auto &materials = rawModel.groups[i].meshMaterials;
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
            glDrawElementsInstanced(GL_TRIANGLES, rawModel.groups[i].mesh.indicesLength, GL_UNSIGNED_INT, nullptr,
                                    groupSize);
        }
    }

    for (auto &it:instancedQueue)
        it.second.clear();
    drawing = false;
}

void Renderer3D::flush()
{

}

void Renderer3D::drawNormals(Model *model)
{
    const RawModel &rawModel = model->rawModel;
    normalDebugShader->bind();
    normalDebugShader->setUniform("projection", camera->getProjectionMatrix());
    normalDebugShader->setUniform("view", camera->getViewMatrix());

    for (int i = 0; i < rawModel.groups.size(); i++)
    {
        rawModel.groups[i].mesh.bind();
        auto &modelGroup = model->modelGroups[i];
        if(!modelGroup.isStatic)
            modelGroup.transform();
        normalDebugShader->setUniform("model", modelGroup.modelMatrix);

        glDrawElements(GL_TRIANGLES, rawModel.groups[i].mesh.indicesLength, GL_UNSIGNED_INT, nullptr);
    }

}

DirLight *shadowDirLight = nullptr;
void Renderer3D::beginShadows(DirLight *dirLight)
{
    if (drawing)
        throw std::runtime_error("Renderer3D::beginShadows(): Cannot begin drawing shadows!");
    if (drawingShadows)
        throw std::runtime_error("Renderer3D::beginShadows(): Cannot begin shadows while already drawing shadows!");

    drawingShadows = true;
    shadowDirLight = dirLight;


}
void Renderer3D::drawShadow(Model *model)
{
    //shadowQueue.insert(model.rawModel.modelID,model);
    uint32_t id = model->rawModel.modelID;
    shadowQueue[id].push_back(model);
}
void Renderer3D::endShadows()
{
    if (!drawingShadows)
        throw std::runtime_error("Renderer3D::endShadows(): beginShadows() must first be called!");
    drawingShadows = false;

    flushShadows();

    shadowDirLight = nullptr;
}

void Renderer3D::flushShadows()
{
    for (auto &it:shadowQueue)
        it.second.clear();
}

void Renderer3D::setCamera(Camera *_camera)
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
