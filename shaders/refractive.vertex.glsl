#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texture_coordinate;
layout (location = 2) in vec3 in_normal;

out vec3 normal;
out vec3 position;

uniform mat4 model;
layout (std140) uniform ViewPerspective
{
    mat4 view;
    mat4 perspective;
};


void main()
{
    normal   = mat3(transpose(inverse(model))) * in_normal;
    position = vec3(model * vec4(in_position, 1.0));
    gl_Position = perspective * view * model * vec4(in_position, 1.0);
}
