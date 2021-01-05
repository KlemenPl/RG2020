//
// Created by klemen on 15/12/2020.
//

#ifndef TOWERDEFENSE_RAWMODEL_H
#define TOWERDEFENSE_RAWMODEL_H

#include <atomic>
#include <string>
#include <vector>
#include "../Core.h"
#include "../graphics/Shader.h"
#include "../graphics/Mesh.h"
#include "Material.h"

class RawModel;

struct Hitbox
{
    glm::vec3 top[4]{};
    glm::vec3 bottom[4]{};
};

struct Group
{
    std::string groupName;

    std::vector<Material> meshMaterials;
    Mesh mesh;
};

class RawModel
{
private:
    inline static std::atomic<uint32_t> ID = 0; // thread safe
public:
    uint32_t modelID;
    std::vector<Group> groups;
    Ref<Shader> modelShader;
    uint32_t cullStrategy;

    RawModel()
    {
        this->modelID = ID.fetch_add(1);
        this->cullStrategy = GL_CCW;
    }

    virtual ~RawModel()
    {
        for (auto &it:groups)
        {
            if (!it.mesh.VBO)
                continue;

            /*
            glDeleteBuffers(GL_ARRAY_BUFFER, &it.mesh.VBO);
            glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &it.mesh.IBO);
            glDeleteVertexArrays(1, &it.mesh.VAO);

            delete it.mesh.vertices;
            delete it.mesh.indices;
             */
            //delete it.meshes;
            it.mesh.dispose();
        }
    }

    void generateMeshes()
    {
        for (auto &it:groups)
        {
            Mesh &mesh = it.mesh;

            // VAO
            glGenVertexArrays(1, &mesh.VAO);
            glBindVertexArray(mesh.VAO);

            // VBO
            glGenBuffers(1, &mesh.VBO);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
            glBufferData(GL_ARRAY_BUFFER, mesh.verticesLength * sizeof(float), mesh.vertices, GL_STATIC_DRAW);

            // configuring pointers
            if (!mesh.hasTexCoords && !mesh.hasNormals)
            {
                // only pos
                glEnableVertexAttribArray(0); // position
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *) 0);

            }
            else if (mesh.hasTexCoords && !mesh.hasNormals)
            {
                // pos, texCoords
                glEnableVertexAttribArray(0); // position
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *) 0);

                glEnableVertexAttribArray(1); // texCoords
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float),
                                      (const void *) (sizeof(float) * 3));

            }
            else if (!mesh.hasTexCoords && mesh.hasNormals)
            {
                // pos, normals
                if (mesh.hasMaterial)
                {
                    glEnableVertexAttribArray(0); // position
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const void *) 0);

                    glEnableVertexAttribArray(1); // normals
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 7 * sizeof(float),
                                          (const void *) (sizeof(float) * 3));

                    glEnableVertexAttribArray(2); // material index
                    glVertexAttribPointer(2, 1, GL_FLOAT, GL_TRUE, 7 * sizeof(float),
                                          (const void *) (sizeof(float) * 6));
                }
                else
                {
                    glEnableVertexAttribArray(0); // position
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *) 0);

                    glEnableVertexAttribArray(1); // normals
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float),
                                          (const void *) (sizeof(float) * 3));
                }
            }
            else
            {
                // pos, texCoords, normals
                glEnableVertexAttribArray(0); // position
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *) 0);

                glEnableVertexAttribArray(1); // texCoords
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 8 * sizeof(float),
                                      (const void *) (sizeof(float) * 3));

                glEnableVertexAttribArray(2); // normals
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float),
                                      (const void *) (sizeof(float) * 5));
            }

            // IBO
            glGenBuffers(1, &mesh.IBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indicesLength * sizeof(uint32_t),
                         mesh.indices, GL_STATIC_DRAW);

            delete[] mesh.vertices;
            delete[] mesh.indices;

            mesh.vertices = nullptr;
            mesh.indices = nullptr;


        }
    }

    // should not be copied!!
    RawModel(const RawModel &) = delete;
    //Model() = default;
    RawModel &operator=(const RawModel &) = delete;


};

#endif //TOWERDEFENSE_RAWMODEL_H
