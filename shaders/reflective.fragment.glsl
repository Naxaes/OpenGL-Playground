#version 330 core

in vec3 normal;
in vec3 position;

out vec4 color;

uniform vec3 camera_position;
uniform samplerCube skybox;

void main()
{
    vec3 I = normalize(position - camera_position);
    vec3 R = reflect(I, normalize(normal));
    color = vec4(texture(skybox, R).rgb, 1.0);
}