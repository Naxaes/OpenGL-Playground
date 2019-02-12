#version 330 core

in vec2 texture_coordinate;

uniform sampler2D image;

out vec4 color;



void main()
{
    color = texture(image, texture_coordinate);
}
