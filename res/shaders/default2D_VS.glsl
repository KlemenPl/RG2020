#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec4 color;
layout (location = 3) in float textureID;

uniform mat4 projectionMatrix;

// passing to fragment shader
out VS_OUT {
    vec2 texCoords;
    float textureID;
    vec4 color;
} vs_out;

void main() {
    vs_out.texCoords = texCoords;
    vs_out.color = color;
    vs_out.textureID = textureID;

    gl_Position = projectionMatrix * vec4(position.xy, 0.0, 1.0);

}
