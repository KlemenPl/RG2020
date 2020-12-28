#version 450 core

out vec4 FragColor;

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

struct Material {
    vec4 Ka;  // ambient
    vec4 Kd;  // diffuse
    vec4 Ks;  // specular
    vec4 NsNi; // shinines, Ni
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

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    Material material;
} fs_in;

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {

    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewPos.xyz - fs_in.fragPos);

    vec3 result = vec3(0, 0, 0);

    // dir lights
    for (int i = 0; i < numDirLights; i++)
        result += calculateDirLight(dirLights[i], norm, viewDir);

    // point lights
    for (int i = 0; i < numPointLights; i++)
        result += calculatePointLight(pointLights[i], norm, fs_in.fragPos, viewDir);

    FragColor = vec4(result, 1.0);
    //FragColor = vec4(numDirLights.x,0.0,0.0, 1.0);
    //FragColor = vec4(dirLights[0].Ka.xyz, 1.0);
    //FragColor = vec4(viewPos.xyz, 1.0);

    //FragColor = vec4(1.0,1.0,1.0,1.0);
    //FragColor = vec4(fs_in.material.Kd, 1.0);
}

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction.xyz);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), fs_in.material.Ns);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 225);
    // combined
    vec3 ambient = light.Ka.xyz * fs_in.material.Ka.xyz;
    vec3 diffuse = light.Kd.xyz * diff * fs_in.material.Kd.xyz;
    vec3 specular = light.Ks.xyz * spec * fs_in.material.Ks.xyz;

    return (ambient + diffuse + specular);
    //return fs_in.material.Ka * light.Ka;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), fs_in.material.NsNi.x);
    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.clq.x + light.clq.y * distance + light.clq.z * (distance * distance));
    // combined
    vec3 ambient = light.Ka.xyz * fs_in.material.Ka.xyz;
    vec3 diffuse = light.Kd.xyz * diff * fs_in.material.Kd.xyz;
    vec3 specular = light.Ks.xyz * spec * fs_in.material.Ks.xyz;

    return (ambient + diffuse + specular);
}
