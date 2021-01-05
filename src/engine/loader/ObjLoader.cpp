//
// Created by klemen on 17/12/2020.
//

#include "Loader.h"
#include "../utils/StringUtils.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <unordered_map>


static glm::vec2 getVec2(const std::string &str)
{
    glm::vec2 res;
    uint32_t index = 0;

    res.x = std::stof(StringUtils::nextPart(str, ' ', index));
    res.y = std::stof(StringUtils::nextPart(str, ' ', index));

    return res;
}

static glm::vec3 getVec3(const std::string &str)
{
    glm::vec3 res;
    uint32_t index = 0;

    res.x = std::stof(StringUtils::nextPart(str, ' ', index));
    res.y = std::stof(StringUtils::nextPart(str, ' ', index));
    res.z = std::stof(StringUtils::nextPart(str, ' ', index));

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

        if (StringUtils::startsWith(line, "newmtl "))
        {
            if (!materialName.empty())
            {
                materials.emplace(materialName, material);
                material = new Material;
            }
            materialName = line.substr(7);
        }
        else if (StringUtils::startsWith(line, "Ka "))
        {
            material->Ka = getVec3(line.substr(3));
        }
        else if (StringUtils::startsWith(line, "Kd "))
        {
            material->Kd = getVec3(line.substr(3));
        }
        else if (StringUtils::startsWith(line, "Ks "))
        {
            material->Ks = getVec3(line.substr(3));
        }
        else if (StringUtils::startsWith(line, "Ns "))
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
                        bool hasTextCoords, bool hasNormals,
                        int materialIndex)
{
    auto it = indicesMap.find(str);
    if (it == indicesMap.end() || !LoaderSettings::useIBO)
    {

        // vertices
        uint32_t index = 0;
        uint32_t vertexIndex = std::stoi(StringUtils::nextPart(str, '/', index)) - 1;

        vertices.push_back(vertexCoords[vertexIndex].x);
        vertices.push_back(vertexCoords[vertexIndex].y);
        vertices.push_back(vertexCoords[vertexIndex].z);

        if (hasTextCoords)
        {
            // texCoords

            uint32_t texIndex = std::stoi(StringUtils::nextPart(str, '/', index)) - 1;

            // adding texCoords
            vertices.push_back(textureCoords[texIndex].x);
            vertices.push_back(textureCoords[texIndex].y);
        }

        if (hasNormals)
        {
            // normals
            uint32_t normalsIndex = std::stoi(StringUtils::nextPart(str, '/', index)) - 1;

            // adding normals
            vertices.push_back(vertexNormals[normalsIndex].x);
            vertices.push_back(vertexNormals[normalsIndex].y);
            vertices.push_back(vertexNormals[normalsIndex].z);
        }

        if (materialIndex > -1)
        {
            // materials
            vertices.push_back(materialIndex);
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

static void processGroup(std::ifstream &objFile, Group *group,
                         std::vector<glm::vec3> &vertexCoords,
                         std::vector<glm::vec3> &vertexNormals,
                         std::vector<glm::vec2> &textureCoords,
                         std::unordered_map<std::string, Material *> &materials)
{

    //static std::vector<glm::vec3> vertexCoords;
    //static std::vector<glm::vec3> vertexNormals;
    //static std::vector<glm::vec2> textureCoords;
    //static Material *currentMaterial = nullptr;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    std::unordered_map<std::string, uint32_t> indicesMap;
    uint32_t indecesIndex = 0;

    bool startedReadingFaces = false;
    bool hasTexCoords = false;
    bool hasNormals = false;
    bool hasMaterials = false;

    Material *currentMaterial = nullptr;
    int materialIndex = -1;


    std::string line;
    int place = 0;

    while (std::getline(objFile, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (StringUtils::startsWith(line, "g "))
        {
            // new group, exiting
            objFile.seekg(place);
            break;
        }
        else if (StringUtils::startsWith(line, "v "))
        {
            // vertex position
            vertexCoords.push_back(getVec3(line.substr(2)));

        }
        else if (StringUtils::startsWith(line, "vt "))
        {
            // texture coords
            textureCoords.push_back(getVec2(line.substr(3)));

        }
        else if (StringUtils::startsWith(line, "vn "))
        {
            // vertex normal
            vertexNormals.push_back(getVec3(line.substr(3)));

        }
        else if (StringUtils::startsWith(line, "usemtl "))
        {
            // change material
            std::string materialName = line.substr(7);
            currentMaterial = materials[materialName];
            group->meshMaterials.emplace_back(*currentMaterial);
            materialIndex++;

        }
        else if (StringUtils::startsWith(line, "f "))
        {
            // start of faces (process mesh)
            if (!startedReadingFaces)
            {
                hasTexCoords = !textureCoords.empty();
                hasNormals = !vertexNormals.empty();
                hasMaterials = currentMaterial != nullptr;
                startedReadingFaces = true;
            }

            // reading face
            std::string faces = line.substr(2);
            uint32_t index = 0;

            std::string face1 = StringUtils::nextPart(faces, ' ', index);
            std::string face2 = StringUtils::nextPart(faces, ' ', index);
            std::string face3 = StringUtils::nextPart(faces, ' ', index);

            //materialIndex = 60;

            processFace(face1,
                        vertexCoords, vertexNormals, textureCoords,
                        vertices, indices,
                        indicesMap, &indecesIndex,
                        hasTexCoords, hasNormals,
                        materialIndex);
            processFace(face2,
                        vertexCoords, vertexNormals, textureCoords,
                        vertices, indices,
                        indicesMap, &indecesIndex,
                        hasTexCoords, hasNormals,
                        materialIndex);
            processFace(face3,
                        vertexCoords, vertexNormals, textureCoords,
                        vertices, indices,
                        indicesMap, &indecesIndex,
                        hasTexCoords, hasNormals,
                        materialIndex);

        }

        place = objFile.tellg(); // last pos
    }

    group->mesh.hasTexCoords = hasTexCoords;
    group->mesh.hasNormals = hasNormals;
    group->mesh.hasMaterial = hasMaterials;

    float *verticesArray = new float[vertices.size()];
    for (int i = 0; i < vertices.size(); i++)
        verticesArray[i] = vertices[i];

    group->mesh.vertices = verticesArray;
    group->mesh.verticesLength = vertices.size();

    uint32_t *indicesArray = new uint32_t[indices.size()];
    for (int i = 0; i < indices.size(); i++)
        indicesArray[i] = indices[i];

    group->mesh.indices = indicesArray;
    group->mesh.indicesLength = indices.size();
}

RawModel *processObject(std::ifstream &objFile, const char *filePath,
                        std::unordered_map<std::string, Material *> materials)
{
    auto *model = new RawModel{};

    std::vector<glm::vec3> vertexCoords;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> textureCoords;

    std::string line;

    std::vector<Group> groups;

    int place = 0;
    bool reading = false;

    while (std::getline(objFile, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (StringUtils::startsWith(line, "mtllib "))
        {
            // load materials
            std::string tmp(filePath);
            int index = tmp.find_last_of('/');
            tmp = tmp.substr(0, index) + '/' + line.substr(7);
            loadMaterials(tmp.c_str(), materials);
        }
        else if (StringUtils::startsWith(line, "g "))
        {
            // new group
            Group group;
            group.groupName = line.substr(2);
            processGroup(objFile, &group, vertexCoords, vertexNormals,
                         textureCoords, materials);
            groups.push_back(std::move(group));

        }
        else if (StringUtils::startsWith(line, "v "))
        {
            // no group specified
            Group group;
            // moving back to prev line
            objFile.seekg(place);
            processGroup(objFile, &group, vertexCoords, vertexNormals,
                         textureCoords, materials);
            group.groupName = "default";
            groups.push_back(std::move(group));
            break;
        }
        else if (StringUtils::startsWith(line, "g "))
        {
            if (reading)
            {
                objFile.seekg(place);
                break;
            }
            reading = true;
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


namespace Loader {
    RawModel *loadOBJ(const char *filePath)
    {
        std::ifstream objFile(filePath);
        if (!objFile.is_open())
        {
            std::cout << "Loader::loadOBJ() could not open file: " << filePath << "." << std::endl;
            return nullptr;
        }
        std::string line;

        auto *model = new RawModel{};

        std::vector<glm::vec3> vertexCoords;
        std::vector<glm::vec3> vertexNormals;
        std::vector<glm::vec2> textureCoords;

        std::unordered_map<std::string, Material *> materials;
        std::vector<Group> groups;

        int place = 0;

        while (std::getline(objFile, line))
        {
            if (!line.empty() && line[line.size() - 1] == '\r')
                line.erase(line.size() - 1);
            if (StringUtils::startsWith(line, "mtllib "))
            {
                // load materials
                std::string tmp(filePath);
                int index = tmp.find_last_of('/');
                tmp = tmp.substr(0, index) + '/' + line.substr(7);
                loadMaterials(tmp.c_str(), materials);
            }
            else if (StringUtils::startsWith(line, "g "))
            {
                // new group
                Group group;
                group.groupName = line.substr(2);
                processGroup(objFile, &group, vertexCoords, vertexNormals,
                             textureCoords, materials);
                groups.push_back(std::move(group));

            }
            else if (StringUtils::startsWith(line, "v "))
            {
                // no group specified
                Group group;
                // moving back to prev line
                objFile.seekg(place);
                processGroup(objFile, &group, vertexCoords, vertexNormals,
                             textureCoords, materials);
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

    void loadOBJObjects(const char *filePath, std::vector<std::pair<std::string, RawModel *>> &output)
    {
        std::ifstream objFile(filePath);
        std::string line;

        std::unordered_map<std::string, Material *> materials;

        while (std::getline(objFile, line))
        {
            if (StringUtils::startsWith(line, "matlib"))
            {
                // load materials
                std::string tmp(filePath);
                int index = tmp.find_last_of('/');
                tmp = tmp.substr(0, index) + '/' + line.substr(7);
                loadMaterials(tmp.c_str(), materials);
            }
            else if (StringUtils::startsWith(line, "o "))
            {
                std::string name = line.substr(2);
                output.emplace_back(name, processObject(objFile, filePath, materials));
            }
            else
            {
                output.emplace_back("", processObject(objFile, filePath, materials));
            }
        }

        objFile.close();
    }
}
