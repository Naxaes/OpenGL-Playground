#version 330 core

in vec2 texture_coordinate;

out vec4 FragColor;

uniform sampler2D image;

void main()
{
    FragColor = vec4(vec3(1.0 - texture(image, texture_coordinate)), 1.0f);
}