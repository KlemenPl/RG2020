#version 450 core

out vec4 FragColor;

struct DirLight {
    vec3 direction; // 4

    vec3 Ka; // 4
    vec3 Kd; // 4
    vec3 Ks; // 4
    // total: 16
};

struct PointLight {
    vec3 position; // 4

    float constant;  // 1
    float linear;    // 1
    float quadratic; // 1
                     // 1

    vec3 Ka; // 4
    vec3 Kd; // 4
    vec3 Ks; // 4
    // total: 20
};

struct Material {
    vec3 Ka;  // ambient
    vec3 Kd;  // diffuse
    vec3 Ks;  // specular
    float Ns; // shininess
    float Ni;
};

layout(std140, binding = 1) uniform lights
{
    int numDirLights; // 4
    DirLight dirLights[]; // numDirLights * 16

    int numPointLights; // 4
    PointLight pointLights[]; // numPointLights * 20
};

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    Material material;
} fs_in;

//layout(std430, binding = 0) buffer MaterialsSSBO
//{
//    Material[] materials;
//};

#define MAX_DIR_LIGHTS 2
#define MAX_POINT_LIGHTS 5

uniform vec3 viewPos; // camera position
uniform int NUM_DIR_LIGHTS;
uniform int NUM_POINT_LIGHTS;
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_DIR_LIGHTS];

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {

    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);

    vec3 result = vec3(0, 0, 0);

    // dir lights
    for (int i = 0; i < NUM_DIR_LIGHTS; i++)
        result += calculateDirLight(dirLights[i], norm, viewDir);

    // point lights
    //for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    //    result += calculatePointLight(pointLights[i], norm, fs_in.fragPos, viewDir);

    FragColor = vec4(result, 1.0);
    //FragColor = vec4(1.0,1.0,1.0,1.0);
    //FragColor = vec4(fs_in.material.Kd, 1.0);
}

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), fs_in.material.Ns);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 225);
    // combined
    vec3 ambient = light.Ka * fs_in.material.Ka;
    vec3 diffuse = light.Kd * diff * fs_in.material.Kd;
    vec3 specular = light.Ks * spec * fs_in.material.Ks;

    return (ambient + diffuse + specular);
    //return fs_in.material.Ka * light.Ka;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), fs_in.material.Ns);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combined
    vec3 ambient = light.Ka * fs_in.material.Ka;
    vec3 diffuse = light.Kd * diff * fs_in.material.Kd;
    vec3 specular = light.Ks * spec * fs_in.material.Ks;

    return (ambient + diffuse + specular);
}
