#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
} fs_out;

uniform mat4 model;
uniform mat4 combined;

void main()
{
    fs_out.fragPos = vec3(model * vec4(position, 1.0));
    fs_out.normal  = mat3(transpose(inverse(model))) * normal;

    gl_Position = combined * vec4(fs_out.fragPos, 1.0);
}