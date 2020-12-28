#version 450 core

out vec4 FragColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec3 colour;
} fs_in;

void main() {
    //FragColor = vec4(fs_in.colour, 1.0);
    FragColor = vec4(1.0,1.0,1.0,1.0);
}
