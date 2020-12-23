#version 450 core

#define blinn 0

out vec4 FragColor;

struct Material {
    vec3 Ka;  // ambient
    vec3 Kd;  // diffuse
    vec3 Ks;  // specular
    float Ns; // shininess
};

struct Light {
    vec3 pos;

    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
};

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
} fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{

    // ambient
    vec3 ambient = light.Ka * material.Ka;

    // diffuse
    vec3  norm     = normalize(fs_in.normal);
    vec3  lightDir = normalize(light.pos - fs_in.fragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = light.Kd * (diff * material.Kd);

    // specular
    vec3  viewDir      = normalize(viewPos - fs_in.fragPos);
    //vec3  reflectedDir = reflect(lightDir, norm);

#if blinn
    vec3  halfwayDir   = normalize(lightDir + viewDir);
    float spec         = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
#else
    vec3  reflectedDir = reflect(lightDir, norm);
    float spec         = pow(max(dot(viewDir, reflectedDir), 0.0), material.Ns);
#endif

    vec3  specular     = light.Ks * (spec * material.Ks);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}