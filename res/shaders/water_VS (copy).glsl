#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 inTextCoords;

layout(std140, binding = 0) uniform vpMatrices
{
    mat4 projection;
    mat4 view;
    mat4 combined;
};


out VS_OUT {
    vec2 textCoords;
    vec4 clipSpace;
    vec3 toCameraVector;
} fs_out;

uniform vec3 camPos;


const float tiling = 5.0f;

void main() {
    //textCoords = vec2(inTextCoords.x / 2.0 + 0.5, position.y / 2.0 + 0.5) * tiling;
    fs_out.textCoords = inTextCoords * tiling;
    //textCoords = vec2(inTextCoords.x, position.y) * tiling;
    fs_out.clipSpace = vec4(position, 1.0);
    gl_Position = combined * fs_out.clipSpace;
    fs_out.toCameraVector = position - camPos.xyz;
}