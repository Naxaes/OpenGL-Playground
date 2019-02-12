#version 330 core

in vec2 texture_coordinate;

out vec4 FragColor;

uniform sampler2D image;

const float offset = 1.0 / 300.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
    );

    float kernel[9] = float[](
       1.0f,  1.0f,  1.0f,
       1.0f, -8.0f,  1.0f,
       1.0f,  1.0f,  1.0f
    );

    vec3 smaple_texture[9];
    for(int i = 0; i < 9; i++)
        smaple_texture[i] = vec3(texture(image, texture_coordinate.st + offsets[i]));

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++)
        color += smaple_texture[i] * kernel[i];

    FragColor = vec4(color, 1.0);
}