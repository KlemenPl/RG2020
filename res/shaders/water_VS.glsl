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
    vec4 clipSpace;
    vec2 textCoords;
    vec3 toCameraVector;
} fs_out;

uniform vec3 camPos;
uniform vec3 lightPos;


const float tiling = 8.0f;

void main() {
    vec4 worldPos = vec4(position, 1.0);
    fs_out.clipSpace = combined * worldPos;
    gl_Position = fs_out.clipSpace;
    fs_out.textCoords = vec2(inTextCoords.x / 2.0 + 0.5, inTextCoords.y / 2.0 + 0.5) * tiling;
    //fs_out.textCoords = vec2(inTextCoords.x, inTextCoords.y) * tiling;
    fs_out.toCameraVector = camPos - worldPos.xyz;
}