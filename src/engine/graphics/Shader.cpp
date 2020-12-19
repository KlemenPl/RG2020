//
// Created by klemen on 04/12/2020.
//

#include <glad/glad.h>
#include <iostream>
#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
    // cleaning up
    glDeleteProgram(ID);
}


Shader &Shader::bind()
{
    glUseProgram(ID);

    return *this;
}

uint32_t Shader::getUniformLocation(const std::string &name)
{
    if (uniformCache.find(name) != uniformCache.end())
        // in our cache
        return uniformCache[name];
    else
    {
        // not in our cache, caching and returning
        uint32_t location = glGetUniformLocation(ID, name.c_str());
        uniformCache[name] = location;
        return location;
    }
}

/*
 * Compiles shader from null point terminated strings (c_str).
 * Will throw runtime exception, if it fails to validate.
 */

void Shader::compile(const char *vsSource, const char *fsSource, const char *gsSource)
{
    uint32_t vertexID, fragmentID, geometryID;

    // vertex shader
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vsSource, NULL);
    glCompileShader(vertexID);

    if (!validate(vertexID, VERTEX))
        throw std::runtime_error("Shader::compile Vertex Shader failed to validate.");

    // fragment shader
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fsSource, NULL);
    glCompileShader(fragmentID);

    if (!validate(fragmentID, FRAGMENT))
        throw std::runtime_error("Shader::compile Fragment Shader failed to validate.");

    // geometry shader
    if (gsSource != nullptr)
    {
        geometryID = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryID, 1, &gsSource, NULL);
        glCompileShader(geometryID);

        if (!validate(geometryID, GEOMETRY))
            throw std::runtime_error("Shader::compile Geometry Shader failed to validate.");
    }


    ID = glCreateProgram();
    glAttachShader(ID, vertexID);
    glAttachShader(ID, fragmentID);
    if (gsSource != nullptr)
        glAttachShader(ID, geometryID);
    glLinkProgram(ID);

    //glDeleteShader(vertexID);
    //glDeleteShader(fragmentID);
    //if (gsSource != nullptr)
    //    glDeleteShader(geometryID);

    if (!validate(ID, SHADER_PROGRAM))
        throw std::runtime_error("Shader::compile Shader program failed to validate.");
}

bool Shader::validate(uint32_t object, ShaderType type)
{
    int status;
    if (type == SHADER_PROGRAM)
    {
        glGetProgramiv(object, GL_LINK_STATUS, &status);

        if (!status)
        {
            int errorLength;
            glGetProgramiv(object, GL_INFO_LOG_LENGTH, &errorLength);
            char errorMessage[errorLength];
            glGetProgramInfoLog(object, errorLength, &errorLength, errorMessage);
            std::cout << "[ERROR::SHADER]\t[Link-time error]:\t";
            std::cout << errorMessage << std::endl;

            return false;
        }
    } else
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &status);

        if (!status)
        {
            int errorLength;
            glGetShaderiv(object, GL_INFO_LOG_LENGTH, &errorLength);

            // message
            char errorMessage[errorLength];
            glGetShaderInfoLog(object, errorLength, &errorLength, errorMessage);
            std::cout << "[ERROR::SHADER]\t";
            if (type == VERTEX)
                std::cout << "[Vertex Shader Compile-timer error]:\n";
            else if (type == FRAGMENT)
                std::cout << "[Fragment Shader Compile-timer error]:\n";
            else if (type == GEOMETRY)
                std::cout << "[Geometry Shader Compile-timer error]:\n";
            std::cout << errorMessage << std::endl;

            return false;
        }
    }

    return true;
}
void Shader::setUniform(const std::string &name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform(const std::string &name, int amount, float *values)
{
    glUniform1fv(getUniformLocation(name), amount, values);
}
void Shader::setUniform(const std::string &name, int amount, int *values)
{
    glUniform1iv(getUniformLocation(name), amount, values);
}

void Shader::setUniform(const std::string &name, int v1, int v2)
{
    glUniform2i(getUniformLocation(name), v1, v2);
}

void Shader::setUniform(const std::string &name, const glm::mat2 &mat2f, bool transpose)
{
    glUniformMatrix2fv(getUniformLocation(name), 1, transpose, &mat2f[0][0]);
}
void Shader::setUniform(const std::string &name, const glm::mat3 &mat3f, bool transpose)
{
    glUniformMatrix2fv(getUniformLocation(name), 1, transpose, &mat3f[0][0]);
}
void Shader::setUniform(const std::string &name, const glm::mat4 &mat4f, bool transpose)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, transpose, &mat4f[0][0]);
}

void Shader::setUniform(const std::string &name, glm::vec1 &v)
{
    glUniform1fv(getUniformLocation(name), 1, &v[0]);
}
void Shader::setUniform(const std::string &name, glm::vec2 &v)
{
    glUniform2fv(getUniformLocation(name), 1, &v[0]);
}
void Shader::setUniform(const std::string &name, glm::vec3 &v)
{
    glUniform3fv(getUniformLocation(name), 1, &v[0]);
}
void Shader::setUniform(const std::string &name, glm::vec4 &v)
{
    glUniform4fv(getUniformLocation(name), 1, &v[0]);
}

void Shader::setUniformMaterial(const std::string &name, const Material &material)
{
    glUniform3fv(getUniformLocation(name + ".Ka"), 1, &material.Ka.x);
    glUniform3fv(getUniformLocation(name + ".Kd"), 1, &material.Kd.x);
    glUniform3fv(getUniformLocation(name + ".Ks"), 1, &material.Ks.x);

    glUniform1f(getUniformLocation(name + ".Ns"), material.Ns);
}

void Shader::setUniformMaterial2(const std::string &name, const Material &material)
{
    glUniform3fv(getUniformLocation(name + ".Ka"), 1, &material.Ka.x);
    glUniform3fv(getUniformLocation(name + ".Kd"), 1, &material.Kd.x);
    glUniform3fv(getUniformLocation(name + ".Ks"), 1, &material.Ks.x);

    //glUniform1f(getUniformLocation(name + ".Ns"), material.Ns);
}