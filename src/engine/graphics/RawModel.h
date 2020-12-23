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

struct Group
{
    std::string groupName;
    const RawModel *parent;

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
            it.parent = this;
                if (it.meshes[i].VBO == 0)
                    continue;

                glDeleteBuffers(GL_ARRAY_BUFFER, &it.meshes[i].VBO);
                glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &it.meshes[i].IBO);
                glDeleteVertexArrays(1, &it.meshes[i].VAO);

                delete it.meshes[i].vertices;
                delete it.meshes[i].indices;
            //delete it.meshes;
        }
    }

    void generateMeshes()
    {
        for (auto &it:groups)
        {
            for (int i = 0; i < it.numMeshes; i++)
            {
                Mesh &mesh = it.meshes[i];

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

                } else if (mesh.hasTexCoords && !mesh.hasNormals)
                {
                    // pos, texCoords
                    glEnableVertexAttribArray(0); // position
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *) 0);

                    glEnableVertexAttribArray(1); // texCoords
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float),
                                          (const void *) (sizeof(float) * 3));

                } else if (!mesh.hasTexCoords && mesh.hasNormals)
                {
                    // pos, normals
                    glEnableVertexAttribArray(0); // position
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *) 0);

                    glEnableVertexAttribArray(1); // normals
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float),
                                          (const void *) (sizeof(float) * 3));

                } else
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


            }
        }
    }

    // should not be copied!!
    RawModel(const RawModel &) = delete;
    //Model() = default;
    RawModel &operator=(const RawModel &) = delete;


};

#endif //TOWERDEFENSE_RAWMODEL_H
