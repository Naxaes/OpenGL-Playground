#version 330 core

in Shared {
    // in vec3 position;
    in vec2 texture_coordinate;
} fs_in;

uniform sampler2D image;

out vec4 color;

void main() {
    color = texture(image, fs_in.texture_coordinate);
}
