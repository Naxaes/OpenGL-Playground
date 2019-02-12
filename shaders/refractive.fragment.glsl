#version 330 core

in vec3 normal;
in vec3 position;

out vec4 color;

uniform vec3 camera_position;
uniform samplerCube skybox;

void main()
{
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(position - camera_position);
    vec3 R = refract(I, normalize(normal), ratio);
    color = vec4(texture(skybox, R).rgb, 1.0);
}