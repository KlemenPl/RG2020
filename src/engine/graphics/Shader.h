//
// Created by klemen on 04/12/2020.
//

#ifndef TOWERDEFENSE_SHADER_H
#define TOWERDEFENSE_SHADER_H

#include <unordered_map>
#include <cstring>
#include <string>
#include "../math/Matrix.h"

enum ShaderType :char{
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    SHADER_PROGRAM
};

struct ShaderSourceArgument {
    ShaderType shaderType;
    std::string identifier;
    std::string argument;


};

class Shader
{
private:

    // to minimize number of calls to opengl
    std::unordered_map<std::string, uint32_t> uniformCache;

    // returns uniform location for shader (will also cache invalid uniforms)
    uint32_t getUniformLocation(const std::string &name);

public:
    // constructor
    Shader();

    // binds shader, and returns refrence for chaining
    Shader& bind();

    void compile(const char *vsSource, const char *fsSource, const char *gsSource = nullptr);

    bool validate(uint32_t object, ShaderType type);

    void setUniform(const std::string &name, float value);

    void setUniform(const std::string &name, int amount, float *values);
    void setUniform(const std::string &name, int amount, int *values);

    void setUniform(const std::string &name, int v1, int v2);

    void setUniform(const std::string &name, const Mat4f& mat4F, bool transpose = false);

    void dispose() const;

    uint32_t ID;
};


#endif //TOWERDEFENSE_SHADER_H
