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
    vec3 colour;
} fs_in;

vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 viewDir = normalize(viewPos.xyz - fs_in.fragPos);

    vec3 result = fs_in.colour;

    // point lights
    //for (int i = 0; i < numPointLights; i++)
    //    result += calculatePointLight(pointLights[i], fs_in.fragPos, viewDir);

    //FragColor = vec4(fs_in.colour, 1.0);
    FragColor = vec4(result, 1.0);
}

vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse
    float diff = max(dot(fs_in.normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, fs_in.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), fs_in.colour.x);
    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.clq.x + light.clq.y * distance + light.clq.z * (distance * distance));
    // combined
    vec3 ambient = light.Ka.xyz * fs_in.colour.xyz;
    vec3 diffuse = light.Kd.xyz * diff * fs_in.colour.xyz;
    vec3 specular = light.Ks.xyz * spec * fs_in.colour.xyz;

    //return (ambient + diffuse + specular);
    return ambient + diffuse;
}