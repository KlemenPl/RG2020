//
// Created by klemen on 17/12/2020.
//

#include "Loader.h"
#include <fstream>
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
    auto *material = new Material{};
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
        }
        else if (startsWith(line, "Ka "))
        {
            material->Ka = getVec3(line.substr(3));
        }
        else if (startsWith(line, "Kd "))
        {
            material->Kd = getVec3(line.substr(3));
        }
        else if (startsWith(line, "Ks "))
        {
            material->Ks = getVec3(line.substr(3));
        }
        else if (startsWith(line, "Ns "))
        {
            material->Ns = std::stof(line.substr(3));
        }
    }

    if (!materialName.empty())
        materials.emplace(materialName, material);
    else
        delete material;

    matFile.close();
}

static void processFace(const std::string &str,
                        std::vector<glm::vec3> &vertexCoords,
                        std::vector<glm::vec3> &vertexNormals,
                        std::vector<glm::vec2> &textureCoords,
                        std::vector<float> &vertices,
                        std::vector<uint32_t> &indices,
                        std::unordered_map<std::string, uint32_t> &indicesMap,
                        uint32_t *indicesIndex,
                        bool hasTextCoords, bool hasNormals)
{
    auto it = indicesMap.find(str);
    if (it == indicesMap.end())
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

        }
        else if (!hasTextCoords)
        {
            // vertices and normals, no texcoords
            uint32_t index = 0;
            uint32_t vertexIndex = std::stoi(nextPart(str, '/', index)) - 1;
            //nextPart(str, '/', index); // can ignore textures part
            uint32_t normalsIndex = std::stoi(nextPart(str, '/', index)) - 1;

            // adding vertices
            vertices.push_back(vertexCoords[vertexIndex].x);
            vertices.push_back(vertexCoords[vertexIndex].y);
            vertices.push_back(vertexCoords[vertexIndex].z);

            // adding normals
            vertices.push_back(vertexNormals[normalsIndex].x);
            vertices.push_back(vertexNormals[normalsIndex].y);
            vertices.push_back(vertexNormals[normalsIndex].z);

        }
        else if (!hasNormals)
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

        }
        else
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

        }

        indices.push_back(*indicesIndex);
        indicesMap[str] = *indicesIndex;
        (*indicesIndex)++;


    }
    else
    {
        // vertex already written
        indices.push_back(it->second);
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
    uint32_t indecesIndex = 0;

    std::string line;
    int place = objFile.tellg();

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
                        indicesMap, &indecesIndex,
                        hasTextCoords, hasNormals);
            processFace(face2,
                        vertexCoords, vertexNormals, textureCoords,
                        vertices, indices,
                        indicesMap, &indecesIndex,
                        hasTextCoords, hasNormals);
            processFace(face3,
                        vertexCoords, vertexNormals, textureCoords,
                        vertices, indices,
                        indicesMap, &indecesIndex,
                        hasTextCoords, hasNormals);
        }
        else
        {
            objFile.seekg(place);
            break;
        }

        place = objFile.tellg();
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
                         std::vector<glm::vec3> &vertexCoords,
                         std::vector<glm::vec3> &vertexNormals,
                         std::vector<glm::vec2> &textureCoords,
                         std::unordered_map<std::string, Material *> &materials,
                         Material *currentMaterial)
{

    std::vector<Mesh> meshes;

    // fixme: this only works when loading only 1 model
    //static std::vector<glm::vec3> vertexCoords;
    //static std::vector<glm::vec3> vertexNormals;
    //static std::vector<glm::vec2> textureCoords;
    //static Material *currentMaterial = nullptr;

    std::string line;
    int place = 0;


    while (std::getline(objFile, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (startsWith(line, "g "))
        {
            // new group, exiting todo
            objFile.seekg(place);
            break;
        }
        else if (startsWith(line, "v "))
        {
            // vertex position
            vertexCoords.push_back(getVec3(line.substr(2)));

        }
        else if (startsWith(line, "vt "))
        {
            // texture coords
            textureCoords.push_back(getVec2(line.substr(3)));

        }
        else if (startsWith(line, "vn "))
        {
            // vertex normal
            vertexNormals.push_back(getVec3(line.substr(3)));

        }
        else if (startsWith(line, "usemtl "))
        {
            // change material
            std::string a = line.substr(7);
            currentMaterial = materials[line.substr(7)];

        }
        else if (startsWith(line, "f "))
        {
            // start of faces (process mesh)
            objFile.seekg(place);

            bool hasTexCoords = !textureCoords.empty();
            bool hasNormals = !vertexNormals.empty();

            Mesh mesh;
            if (currentMaterial)
                mesh.defaultMaterial = Material(*currentMaterial);
            mesh.hasNormals = hasNormals;
            mesh.hasTexCoords = hasTexCoords;
            processMesh(objFile, &mesh, vertexCoords, vertexNormals, textureCoords, hasTexCoords, hasNormals);
            meshes.push_back(std::move(mesh));

        }

        place = objFile.tellg(); // last pos
    }

    //group->numMeshes = meshes.size();
    //Mesh *meshesArray = new Mesh[meshes.size()];
    for (auto &it:meshes)
        group->meshes.push_back(std::move(it));
    group->numMeshes = group->meshes.size();
    //group->meshes = meshesArray;
}


namespace Loader {
    RawModel *loadOBJ(const char *filePath)
    {
        auto *model = new RawModel{};

        std::vector<glm::vec3> vertexCoords;
        std::vector<glm::vec3> vertexNormals;
        std::vector<glm::vec2> textureCoords;
        Material *currentMaterial = nullptr;

        std::ifstream objFile(filePath);
        std::string line;

        std::unordered_map<std::string, Material *> materials;
        std::vector<Group> groups;

        int place = 0;

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
            }
            else if (startsWith(line, "g "))
            {
                // new group
                Group group;
                group.groupName = line.substr(2);
                processGroup(objFile, &group, vertexCoords, vertexNormals,
                             textureCoords, materials, currentMaterial);
                groups.push_back(std::move(group));

            }
            else if (startsWith(line, "v "))
            {
                // no group specified
                Group group;
                // moving back to prev line
                objFile.seekg(place);
                processGroup(objFile, &group, vertexCoords, vertexNormals,
                             textureCoords, materials, currentMaterial);
                group.groupName = "default";
                groups.push_back(std::move(group));
                break;
            }

            place = objFile.tellg(); // last pos
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