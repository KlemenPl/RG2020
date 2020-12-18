//
// Created by klemen on 17/12/2020.
//

#include "Loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <unordered_map>

static bool startsWith(const std::string &str, const std::string &start)
{
    unsigned int length = start.length();

    if (length > str.length())
        return false;

    for (int i = 0; i < length; i++)
        if (str[i] != start[i])
            return false;

    return true;
}

static std::string nextPart(const std::string &str, const char separator, uint32_t &index)
{
    int i;
    for (i = index; i < str.length(); i++)
        if (str[i] == separator)
            break;
    std::string part = str.substr(index, i - index);
    for (; i < str.length(); i++)
        if (str[i] != separator)
            break;
    index = i;
    return part;
}

static glm::vec2 getVec2(const std::string &str)
{
    glm::vec2 res;
    uint32_t index = 0;

    res.x = std::stof(nextPart(str, ' ', index));
    res.y = std::stof(nextPart(str, ' ', index));

    return res;
}

static glm::vec3 getVec3(const std::string &str)
{
    glm::vec3 res;
    uint32_t index = 0;

    res.x = std::stof(nextPart(str, ' ', index));
    res.y = std::stof(nextPart(str, ' ', index));
    res.z = std::stof(nextPart(str, ' ', index));

    return res;
}

static void loadMaterials(const char *filePath, std::unordered_map<std::string, Material *> &materials)
{
    std::string materialName;
    Material *material = new Material;
    std::ifstream matFile(filePath);
    std::string line;

    while (std::getline(matFile, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (startsWith(line, "newmtl "))
        {
            if (!materialName.empty())
            {
                materials.emplace(materialName, material);
                material = new Material;
            }
            materialName = line.substr(7);
        } else if (startsWith(line, "Ka "))
        {
            material->Ka = getVec3(line.substr(3));
        } else if (startsWith(line, "Kd "))
        {
            material->Kd = getVec3(line.substr(3));
        } else if (startsWith(line, "Ks "))
        {
            material->Ks = getVec3(line.substr(3));
        } else if (startsWith(line, "Ns "))
        {
            material->Ns = std::stof(line.substr(3));
        }
    }

    if (!materialName.empty())
        materials.emplace(materialName, material);
    matFile.close();
}

static void processFace(const std::string &str,
                        std::vector<glm::vec3> &vertexCoords,
                        std::vector<glm::vec3> &vertexNormals,
                        std::vector<glm::vec2> &textureCoords,
                        std::vector<float> &vertices,
                        std::vector<uint32_t> &indices,
                        std::unordered_map<std::string, uint32_t> &indicesMap,
                        bool hasTextCoords, bool hasNormals)
{
    if (indicesMap.find(str) == indicesMap.end())
    {
        // vertex not in vertices, adding it
        if (!hasTextCoords && !hasNormals)
        {
            // only vertices
            uint32_t index = 0;
            uint32_t vertexIndex = std::stoi(nextPart(str, '/', index)) - 1;

            vertices.push_back(vertexCoords[vertexIndex].x);
            vertices.push_back(vertexCoords[vertexIndex].y);
            vertices.push_back(vertexCoords[vertexIndex].z);

            indices.push_back(indices.size());
            indicesMap[str] = indices.size() - 1;


        } else if (!hasTextCoords && hasNormals)
        {
            // vertices and normals, no texcoords
            uint32_t index = 0;
            uint32_t vertexIndex = std::stoi(nextPart(str, '/', index)) - 1;
            nextPart(str, '/', index); // can ignore textures part
            uint32_t normalsIndex = std::stoi(nextPart(str, '/', index)) - 1;

            // adding vertices
            vertices.push_back(vertexCoords[vertexIndex].x);
            vertices.push_back(vertexCoords[vertexIndex].y);
            vertices.push_back(vertexCoords[vertexIndex].z);

            // adding normals
            vertices.push_back(vertexNormals[normalsIndex].x);
            vertices.push_back(vertexNormals[normalsIndex].y);
            vertices.push_back(vertexNormals[normalsIndex].z);

            indices.push_back(indices.size());
            indicesMap[str] = indices.size() - 1;
        } else if (hasTextCoords && !hasNormals)
        {
            // vertices and texCoords, no normals
            uint32_t index = 0;
            uint32_t vertexIndex = std::stoi(nextPart(str, '/', index)) - 1;
            uint32_t texIndex = std::stoi(nextPart(str, '/', index)) - 1;

            // adding vertices
            vertices.push_back(vertexCoords[vertexIndex].x);
            vertices.push_back(vertexCoords[vertexIndex].y);
            vertices.push_back(vertexCoords[vertexIndex].z);

            // adding texCoords
            vertices.push_back(textureCoords[texIndex].x);
            vertices.push_back(textureCoords[texIndex].y);

            indices.push_back(indices.size());
            indicesMap[str] = indices.size() - 1;
        } else if (hasNormals && hasTextCoords)
        {
            // vertices, normals, texCoords
            uint32_t index = 0;
            uint32_t vertexIndex = std::stoi(nextPart(str, '/', index)) - 1;
            uint32_t texIndex = std::stoi(nextPart(str, '/', index)) - 1;
            uint32_t normalsIndex = std::stoi(nextPart(str, '/', index)) - 1;

            // adding vertices
            vertices.push_back(vertexCoords[vertexIndex].x);
            vertices.push_back(vertexCoords[vertexIndex].y);
            vertices.push_back(vertexCoords[vertexIndex].z);

            // adding texCoords
            vertices.push_back(textureCoords[texIndex].x);
            vertices.push_back(textureCoords[texIndex].y);

            // adding normals
            vertices.push_back(vertexNormals[normalsIndex].x);
            vertices.push_back(vertexNormals[normalsIndex].y);
            vertices.push_back(vertexNormals[normalsIndex].z);

            indices.push_back(indices.size());
            indicesMap[str] = indices.size() - 1;
        }


    } else
    {
        // vertex already written
        indices.push_back(indicesMap[str]);
    }

}

static void processMesh(std::ifstream &objFile,
                        Mesh *mesh,
                        std::vector<glm::vec3> &vertexCoords,
                        std::vector<glm::vec3> &vertexNormals,
                        std::vector<glm::vec2> &textureCoords,
                        bool hasTextCoords = true, bool hasNormals = true)
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    std::unordered_map<std::string, uint32_t> indicesMap;

    std::string line;
    while (std::getline(objFile, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (startsWith(line, "f "))
        {
            // reading face
            std::string faces = line.substr(2);
            uint32_t index = 0;

            std::string face1 = nextPart(faces, ' ', index);
            std::string face2 = nextPart(faces, ' ', index);
            std::string face3 = nextPart(faces, ' ', index);

            processFace(face1,
                        vertexCoords, vertexNormals, textureCoords,
                        vertices, indices,
                        indicesMap,
                        hasTextCoords,
                        hasNormals);
            processFace(face2,
                        vertexCoords, vertexNormals, textureCoords,
                        vertices, indices,
                        indicesMap,
                        hasTextCoords,
                        hasNormals);
            processFace(face3,
                        vertexCoords, vertexNormals, textureCoords,
                        vertices, indices,
                        indicesMap,
                        hasTextCoords,
                        hasNormals);
        } else
            break;

    }

    float *verticesArray = new float[vertices.size()];
    for (int i = 0; i < vertices.size(); i++)
        verticesArray[i] = vertices.at(i);

    mesh->vertices = verticesArray;
    mesh->verticesLength = vertices.size();

    uint32_t *indicesArray = new uint32_t[indices.size()];
    for (int i = 0; i < indices.size(); i++)
        indicesArray[i] = indices.at(i);

    mesh->indices = indicesArray;
    mesh->indicesLength = indices.size();

    // generating VAO, VBO, IBO
}

static void processGroup(std::ifstream &objFile, Group *group,
                         std::unordered_map<std::string, Material *> &materials)
{

    std::vector<Mesh> meshes;

    std::vector<glm::vec3> vertexCoords;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> textureCoords;

    std::string line;
    int place = 0;

    Material *currentMaterial;

    while (std::getline(objFile, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (startsWith(line, "g "))
        {
            // new group, exiting
            objFile.seekg(place);
            break;
        } else if (startsWith(line, "v "))
        {
            // vertex position
            vertexCoords.push_back(getVec3(line.substr(2)));

        } else if (startsWith(line, "vt "))
        {
            // texture coords
            textureCoords.push_back(getVec2(line.substr(3)));

        } else if (startsWith(line, "vn "))
        {
            // vertex normal
            vertexNormals.push_back(getVec3(line.substr(3)));

        } else if (startsWith(line, "usemtl "))
        {
            // change material
            currentMaterial = materials[line.substr(7)];

        } else if (startsWith(line, "f "))
        {
            // start of faces (process mesh)
            objFile.seekg(place);

            Mesh mesh;
            mesh.material = Material(*currentMaterial);
            processMesh(objFile, &mesh, vertexCoords, vertexNormals, textureCoords);
            meshes.push_back(mesh);
        }

        place = objFile.tellg(); // last pos
    }

    group->numMeshes = meshes.size();
    Mesh *meshesArray = new Mesh[meshes.size()];
    for (auto &it:meshes)
        group->meshes.push_back(std::move(it));
    //group->meshes = meshesArray;
}


namespace Loader {
    Model *loadOBJ(const char *filePath, bool recalculateNormals, bool ignoreVertexNormals)
    {
        Model *model = new Model;

        std::ifstream objFile(filePath);
        std::string line;

        std::unordered_map<std::string, Material *> materials;
        std::vector<Group> groups;

        while (std::getline(objFile, line))
        {
            if (!line.empty() && line[line.size() - 1] == '\r')
                line.erase(line.size() - 1);
            if (startsWith(line, "mtllib "))
            {
                // load materials
                std::string tmp(filePath);
                int index = tmp.find_last_of('/');
                tmp = tmp.substr(0, index) + '/' + line.substr(7);
                loadMaterials(tmp.c_str(), materials);
            } else if (startsWith(line, "g "))
            {
                // new group
                Group group;
                group.groupName = line.substr(2);
                processGroup(objFile, &group, materials);
                groups.push_back(group);

            } else if (startsWith(line, "v "))
            {
                // no group specified
                Group group;
                processGroup(objFile, &group, materials);
                group.groupName = "default";
                groups.push_back(group);
                break;
            }
        }

        objFile.close();

        for (auto &it:groups)
            model->groups.push_back(std::move(it));

        // cleaning up materials
        for (auto &it:materials)
            delete it.second;

        return model;
    }
}