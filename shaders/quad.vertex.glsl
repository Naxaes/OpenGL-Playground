#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinate;

uniform mat4 model;
layout (std140) uniform ViewPerspective
{
    mat4 view;
    mat4 perspective;
};

out Shared {
//    out vec3 position;
    out vec2 texture_coordinate;
} vs_out;

void main()
{
    gl_Position = perspective * view * model * vec4(position, 1.0);
    // vs_out.position = vec3(model * vec4(position, 1.0));

    vs_out.texture_coordinate = texture_coordinate;
}