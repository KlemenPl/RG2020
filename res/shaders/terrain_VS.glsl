#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colour;

layout(std140, binding = 0) uniform vpMatrices
{
    mat4 projection;
    mat4 view;
    mat4 combined;
};

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec3 colour;
} fs_out;

void main() {
    fs_out.fragPos = vec3(0,0,0);
    fs_out.normal = normal;
    fs_out.colour = colour.xyz;

    gl_Position = combined * vec4(position,1.0f);
}
