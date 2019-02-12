#version 330 core

in vec2 texture_coordinate;

out vec4 color;

uniform sampler2D blend_map_texture;
uniform sampler2D grass_texture;
uniform sampler2D mud_texture;
uniform sampler2D flowers_texture;
uniform sampler2D path_texture;


void main()
{
    vec4 blend_map_color = texture(blend_map_texture, texture_coordinate);

    vec2 tiled_coordinates = texture_coordinate * 40;
    float background_factor = 1 - (blend_map_color.r + blend_map_color.g + blend_map_color.b);

    vec4 grass_color   = texture(grass_texture,   tiled_coordinates) * background_factor;
    vec4 mud_color     = texture(mud_texture,     tiled_coordinates) * blend_map_color.r;
    vec4 flowers_color = texture(flowers_texture, tiled_coordinates) * blend_map_color.g;
    vec4 path_color    = texture(path_texture,    tiled_coordinates) * blend_map_color.b;

    color = grass_color + mud_color + flowers_color + path_color;
}