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
    vec2 textCoords;
    vec4 clipSpace;
    vec3 toCameraVector;
} fs_in;


uniform sampler2D reflectionSample;
uniform sampler2D refractionSample;
uniform sampler2D dudvSample;
uniform sampler2D normalSample;
uniform sampler2D depthSample;

uniform float moveFactor;

const float waveStrength = 0.008f;

const float shineDamper = 20.0f;
const float reflectivity = 0.8f;

void main() {

    vec2 ndc = (fs_in.clipSpace.xy / fs_in.clipSpace.w) / 2.0 + 0.5;
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

    float near = 0.1f;
    float far = 1000.0f;
    float depth = texture(depthSample, refractTexCoords).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    float waterDepth = floorDistance - waterDistance;

    //vec2 distortion1 = (texture(dudvSample, vec2(textCoords.x + moveFactor, textCoords.y)).rg * 2.0 - 1.0) * waveStrength;
    //vec2 distortion2 = (texture(dudvSample, vec2(-textCoords.x + moveFactor, textCoords.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength;
    //vec2 totalDistortion = distortion1 + distortion2;

    vec2 distortedTexCoords = texture(dudvSample, vec2(fs_in.textCoords.x + moveFactor, fs_in.textCoords.y)).rg * 0.1f;
    distortedTexCoords = fs_in.textCoords + vec2(distortedTexCoords.x , distortedTexCoords.y + moveFactor);
    vec2 totalDistortion = (texture(dudvSample, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength *
    clamp(waterDepth * 20.0, 0.0, 1.0);

    refractTexCoords += totalDistortion;
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    reflectTexCoords += totalDistortion;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, 0.001);

    vec4 reflectColour = texture(reflectionSample, reflectTexCoords);
    vec4 refractColour = texture(refractionSample, refractTexCoords);

    vec3 viewVector = normalize(fs_in.toCameraVector);
    float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
    refractiveFactor = pow(refractiveFactor, 0.25);

    vec4 normalMapColour = texture(normalSample, distortedTexCoords);
    vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
    normal = normalize(normal);

    /*
    vec3 reflectedLight = reflect(normalize(fs_in.toCameraVector), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularHighlights = vec3(0,0,0);

    for(int i = 0; i < numDirLights; i++)
    {
        specularHighlights += (dirLights[0].Ka.xyz * specular * reflectivity);
    }
    */
    vec3 specularHighlights = vec3(0,0,0);
    for(int i = 0; i < 1; i++)
    {
        //vec3 reflectedLight = reflect(normalize(-dirLights[i].direction.xyz), normal);
        //float specular = max(dot(reflectedLight, viewVector), 0.0);
        //specular = pow(specular, shineDamper);
        //vec3 specularHighlights = vec3(0,0,0);
        //specularHighlights += (dirLights[i].Ka.xyz * specular * reflectivity);

        vec3 lightDir = normalize(-dirLights[i].direction.xyz);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 reflectedDir = reflect(-lightDir, normal);
        vec3 viewDir = viewPos.xyz - fs_in.clipSpace.xyz;
        float spec = pow(max(dot(viewDir, reflectedDir), 0.0), reflectivity);
        vec3 diffuse = dirLights[i].Kd.xyz * diff;
        vec3 specular = dirLights[i].Ks.xyz * spec * diff;

        specularHighlights +=  specular;
    }

    FragColor = mix(reflectColour, refractColour, refractiveFactor);
    //FragColor.a = 0.6f;
    FragColor = mix(FragColor, vec4(0.0, 0.25, 0.5, 1.0), 0.55) * vec4(specularHighlights, 1.0f);
    FragColor.a = clamp(waterDepth * 5, 0.0, 1.0);
    //FragColor = normalMapColour;
    //FragColor = vec4(waterDepth/10.0);


}
