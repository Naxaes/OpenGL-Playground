#version 330 core

layout (location = 0) in vec3 position;

out vec3 texture_coordinate;

uniform mat4 perspective;
uniform mat4 skybox_view;

void main()
{
    texture_coordinate = position;
    vec4 pos = perspective * skybox_view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}