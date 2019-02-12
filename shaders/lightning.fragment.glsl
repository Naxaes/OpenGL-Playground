#version 330 core


in Shared {
    vec4 fragment_position_in_light_space;
    vec3 fragment_position;
    vec3 normal;
    vec2 texture_coordinate;
} fs_in;


struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};


#define POINT_LIGHT_COUNT 4


uniform vec3 camera_position;
uniform DirectionalLight directional_light;
uniform PointLight point_light[POINT_LIGHT_COUNT];
uniform Material material;

uniform sampler2D shadow_map;

out vec4 FragColor;


float calculate_shadow(vec4 fragment_position_in_light_space, float bias)
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


vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 view_direction)
{
    vec3 light_direction = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, light_direction), 0.0);

    // Specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);

    // Result
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, fs_in.texture_coordinate));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, fs_in.texture_coordinate));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texture_coordinate));

    // Shadow
    float bias   = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);
    float shadow = calculate_shadow(fs_in.fragment_position_in_light_space, bias);
    vec3  result = (ambient + (1.0 - shadow) * (diffuse + specular));

    return result;
}


vec3 calculate_point_light(PointLight light, vec3 normal, vec3 frag_position, vec3 view_direction)
{
    vec3 light_direction = normalize(light.position - frag_position);

    // Diffuse
    float diff = max(dot(normal, light_direction), 0.0);

    // Specular
    vec3  reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);

    // Attenuation
    float distance    = length(light.position - frag_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Result
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, fs_in.texture_coordinate));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, fs_in.texture_coordinate));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texture_coordinate));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}


void main()
{
    vec3 normal = normalize(fs_in.normal);
    vec3 view_direction = normalize(camera_position - fs_in.fragment_position);

    vec3 result = calculate_directional_light(directional_light, normal, view_direction);

    for (int i = 0; i < POINT_LIGHT_COUNT; i++)
        result += calculate_point_light(point_light[i], normal, fs_in.fragment_position, view_direction);

    //
    //result += calculate_spot_light(spot_light, norm, fs_in.fragment_position, view_direction);


    FragColor = vec4(result, 1.0);
}