#version 450 core

out vec4 FragColor;

struct Material {
    vec4 Ka;  // ambient
    vec4 Kd;  // diffuse
    vec4 Ks;  // specular
    vec4 NsNi; // shinines, Ni
};

uniform vec4 colour;


in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    Material material;
} fs_in;

void main() {

    FragColor = colour;
}