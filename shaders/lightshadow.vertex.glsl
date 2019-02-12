#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinate;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 light_perspective_view;


out Shared {
    vec4 fragment_position_in_light_space;
    vec3 fragment_position;
    vec3 normal;
    vec2 texture_coordinate;
} vs_out;



void main()
{
    gl_Position = perspective * view * model * vec4(position, 1.0);

    vs_out.fragment_position = vec3(model * vec4(position, 1.0));
    vs_out.normal = transpose(inverse(mat3(model))) * normal;
    vs_out.texture_coordinate = texture_coordinate;
    vs_out.fragment_position_in_light_space = light_perspective_view * vec4(vs_out.fragment_position, 1.0);
}
