#version 450 core

out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    float textureID;
    vec4 color;
} fs_in;

uniform sampler2D textures[{TEXTURE_SLOTS}]; // set via ShaderSourceArgument

void main() {
    FragColor = fs_in.color * texture(textures[int(fs_in.textureID)], fs_in.texCoords);
}
