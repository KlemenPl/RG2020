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

struct DirLight {
    vec4 direction; // 4

    vec4 Ka; // 4
    vec4 Kd; // 4
    vec4 Ks; // 4
// total: 16
};

struct PointLight {
    vec4 position; // 4

    vec4 Ka; // 4
    vec4 Kd; // 4
    vec4 Ks; // 4

    vec4 clq; // constant, linear, quadratic

// total: 20
};

#define MAX_DIR_LIGHTS 2
#define MAX_POINT_LIGHTS 5


layout(std140, binding = 2) uniform lights
{
    DirLight dirLights[MAX_DIR_LIGHTS]; // numDirLights * 16
    PointLight pointLights[MAX_POINT_LIGHTS]; // numPointLights * 20

    vec4 viewPos;

    int numDirLights;
    int numPointLights;
};

vec3 calculateDirLight(DirLight light, vec3 normal);

void main() {
    fs_out.fragPos  = position;
    //fs_out.normal   = normal * 2.0 - 1.0;
    fs_out.normal   = normalize(normal * 2.0 - 1.0);
    fs_out.colour   = vec3(0, 0, 0);

    // dir lights
    // calculating in vertex shader instead of fragment
    for (int i = 0; i < numDirLights; i++)
        fs_out.colour += calculateDirLight(dirLights[i], fs_out.normal);

    gl_Position = combined * vec4(position, 1.0);

}

vec3 calculateDirLight(DirLight light, vec3 normal)
{
    // ambient
    vec3 lightDir = normalize(-light.direction.xyz);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // combined
    vec3 ambient = light.Ka.xyz * colour.xyz;
    vec3 diffuse = light.Kd.xyz * diff * colour.xyz;

    return (ambient + diffuse);
}
