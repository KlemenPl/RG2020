//
// Created by klemen on 04/12/2020.
//

#ifndef TOWERDEFENSE_SHADER_H
#define TOWERDEFENSE_SHADER_H

#include <unordered_map>
#include <cstring>
#include <string>
#include <mat4x4.hpp>
#include "Material.h"

enum ShaderType : char
{
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    SHADER_PROGRAM
};

struct ShaderSourceArgument
{
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
    Shader &bind();

    void compile(const char *vsSource, const char *fsSource, const char *gsSource = nullptr);

    bool validate(uint32_t object, ShaderType type);

    // @formatter:off

    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, int amount, float *values);
    void setUniform(const std::string &name, int amount, int *values);
    void setUniform(const std::string &name, int v1, int v2);

    void setUniform(const std::string &name, const glm::mat2& mat2f, bool transpose = false);
    void setUniform(const std::string &name, const glm::mat3& mat3f, bool transpose = false);
    void setUniform(const std::string &name, const glm::mat4& mat4f, bool transpose = false);

    void setUniform(const std::string &name, glm::vec1& v);
    void setUniform(const std::string &name, glm::vec2& v);
    void setUniform(const std::string &name, glm::vec3& v);
    void setUniform(const std::string &name, glm::vec4& v);

    void setUniformMaterial( const std::string& name, const Material& material);
    void setUniformMaterial2(const std::string& name, const Material& material);

    // @formatter:on

    virtual ~Shader();

    uint32_t ID;
};


#endif //TOWERDEFENSE_SHADER_H
