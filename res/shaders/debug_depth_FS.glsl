#version 450 core

out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    float textureID;
    vec4 color;
} fs_in;

uniform sampler2D textures[{TEXTURE_SLOTS}]; // set via ShaderSourceArgument
uniform float near_plane = 0.1;
uniform float far_plane = 1000.0;

void main() {
    float depthValue = texture(textures[int(fs_in.textureID)], fs_in.texCoords).r;
    float z = depthValue * 2.0 - 1.0; // Back to NDC
    float depth = (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));

    FragColor = fs_in.color * vec4(depth, depth, depth, 1.0);
}
