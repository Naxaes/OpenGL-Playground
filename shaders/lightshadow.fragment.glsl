#version 330 core

in Shared {
    vec4 fragment_position_in_light_space;
    vec3 fragment_position;
    vec3 normal;
    vec2 texture_coordinate;
} fs_in;

uniform sampler2D image;
uniform sampler2D shadow_map;

uniform vec3 light_position;
uniform vec3 camera_position;

out vec4 FragColor;


float CalculateShadow(vec4 fragment_position_in_light_space, float bias)
{
    vec3 perspective_coordinates = fragment_position_in_light_space.xyz / fragment_position_in_light_space.w;
    // transform to [0,1] range
    perspective_coordinates = perspective_coordinates * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closest_depth = texture(shadow_map, perspective_coordinates.xy).r;
    // get depth of current fragment from light's perspective
    float current_depth = perspective_coordinates.z;
    // check whether current frag pos is in shadow
    float shadow = current_depth - bias > closest_depth  ? 1.0 : 0.0;

    if (perspective_coordinates.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 color = texture(image, fs_in.texture_coordinate).rgb;
    vec3 normal = normalize(fs_in.normal);
    vec3 light_color = vec3(1.0);

    // AMBIENT
    vec3 ambient = color;

    // DIFFUSE
    vec3  light_direction = normalize(light_position - fs_in.fragment_position.xyz);
    float angle   = max(dot(light_direction, normal), 0.0);
    vec3  diffuse = angle * light_color;

    // SPECULAR
    vec3  view_direction = normalize(camera_position - fs_in.fragment_position.xyz);
    vec3  halfway_direction = normalize(light_direction + view_direction);
    float spec = pow(max(dot(normal, halfway_direction), 0.0), 64.0);
    vec3  specular = spec * light_color;

    // SHADOW
    float bias     = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);
    float shadow   = CalculateShadow(fs_in.fragment_position_in_light_space, bias);
    vec3  lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}