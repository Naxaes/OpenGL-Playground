#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texture_coordinate;
layout (location = 2) in vec3 in_normal;

out vec2 texture_coordinate;



uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;


void main()
{
    texture_coordinate = in_texture_coordinate;
    gl_Position = perspective * view * model * vec4(in_position, 1.0);
}
