#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in float materialIndex;
layout (location = 6) in mat4 instanceMatrix;

struct Material {
    vec4 Ka;    // 0
    vec4 Kd;    // 4
    vec4 Ks;    // 8
    vec4 NsNi;  // 12
    // total: 16
};

layout(std140, binding = 0) uniform vpMatrices
{
    mat4 projection;
    mat4 view;
    mat4 combined;
};

layout(std430, binding = 0) readonly buffer MaterialsSSBO
{
    Material materials[];
} materialData;

//uniform mat4 combined;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    Material material;
} fs_out;

void main() {
    fs_out.fragPos       = vec3(instanceMatrix * vec4(position, 1.0));
    fs_out.normal        = mat3(transpose(inverse(instanceMatrix))) * normal;
    //fs_out.normal = normal;
    //fs_out.materialIndex = materialIndex;
    fs_out.material      = materialData.materials[int(materialIndex)];
    //fs_out.material      = materialData.materials[1];
    //fs_out.material.Kd = vec3(1,1,1);

    gl_Position = combined * vec4(fs_out.fragPos, 1.0);
    //gl_Position = combined * vec4(position, 1.0);
    //gl_Position = vec4(position, 1.0);
}
