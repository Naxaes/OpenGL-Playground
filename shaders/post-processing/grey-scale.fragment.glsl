#version 330 core

in vec2 texture_coordinate;

out vec4 FragColor;

uniform sampler2D image;

void main()
{
    FragColor = texture(image, texture_coordinate);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}