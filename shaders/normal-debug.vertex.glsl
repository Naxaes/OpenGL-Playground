#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 2) in vec3 in_normal;

out VS_OUT {
    vec3 normal;
} vs_out;

layout (std140) uniform ViewPerspective
{
    mat4 view;
    mat4 perspective;
};

uniform mat4 model;

void main()
{
    gl_Position = perspective * view * model * vec4(in_position, 1.0);
    mat3 normal_matrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(perspective * vec4(normal_matrix * in_normal, 0.0)));
}