//
// Created by klemen on 15/12/2020.
//

#ifndef TOWERDEFENSE_MODEL_H
#define TOWERDEFENSE_MODEL_H

#include <atomic>
#include <glm.hpp>
#include <string>
#include <vector>
#include "../Core.h"
#include "../graphics/Shader.h"

struct Material
{
    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    float Ns;

    Material(const glm::vec3 &ka = {1.0f, 1.0f, 1.0f}, // ambient colour
             const glm::vec3 &kd = {1.0f, 1.0f, 1.0f}, // diffuse colour
             const glm::vec3 &ks = {0.0f, 0.0f, 0.0f}, // specular colour
             float ns = 10.0f // specular exponent 0 - 1000
    ) : Ka(ka), Kd(kd), Ks(ks), Ns(ns)
    {}

    Material(const Material &material)
            : Ka(material.Ka),
              Kd(material.Kd),
              Ks(material.Ks),
              Ns(material.Ns)
    {}


};

struct Mesh
{
    Material material;

    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t IBO = 0;

    float *vertices;
    uint32_t *indices;

    uint32_t verticesLength;
    uint32_t indicesLength;
};

struct Group
{
    std::string groupName;

    uint32_t numMeshes;
    std::vector<Mesh> meshes;
};

class Model
{
private:
    inline static std::atomic<uint32_t> ID = 0; // thread safe
public:
    uint32_t modelID;
    std::vector<Group> groups;
    Ref<Shader> modelShader;

    Model()
    {
        this->modelID = ID.fetch_add(1);
    }

    virtual ~Model()
    {
        for (auto &it:groups)
        {
            for (int i = 0; i < it.numMeshes; i++)
            {
                if (it.meshes[i].VBO == 0)
                    continue;

                glDeleteBuffers(GL_ARRAY_BUFFER, &it.meshes[i].VBO);
                glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &it.meshes[i].IBO);
                glDeleteVertexArrays(1, &it.meshes[i].VAO);

                delete it.meshes[i].vertices;
                delete it.meshes[i].indices;
            }
            //delete it.meshes;
        }
    }

    // should not be copied!!
    Model(const Model &) = delete;
    //Model() = default;
    Model &operator=(const Model &) = delete;


};

#endif //TOWERDEFENSE_MODEL_H
