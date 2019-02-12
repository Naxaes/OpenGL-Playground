//
// Created by Ted Klein Bergman on 2019-01-10.
//
#include <glm.hpp>

#include "window.h"
#include "camera.h"
#include "shader.h"
#include "utilities.h"
#include "texture.h"
#include "model.h"
#include "mathematics.h"

constexpr float GRAVITY = -9.82f;

Camera camera{.position = glm::vec3(5, 1.0, 5), .far = 1000.f};
glm::vec3 velocity{};
bool on_ground = false;

bool  first_mouse = true;
float yaw   =  90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float last_x = 800.0f / 2.0f;
float last_y = 600.0f / 2.0f;
float fov   =  45.0f;



void handle_input(Window window, Camera& camera)
{
    static float speed = 1.0f;

    if (glfwGetKey(window.id, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.id, true);
    if (glfwGetKey(window.id, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += speed * camera.front;
    if (glfwGetKey(window.id, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= speed * camera.front;
    if (glfwGetKey(window.id, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * speed;
    if (glfwGetKey(window.id, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * speed;
    if (glfwGetKey(window.id, GLFW_KEY_SPACE) == GLFW_PRESS && on_ground)
        velocity.y += 5.0f;
    if (glfwGetKey(window.id, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
    if (glfwGetKey(window.id, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed = 3.0f;
    if (glfwGetKey(window.id, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        speed = 1.0f;
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
    if (first_mouse)
    {
        last_x = x;
        last_y = y;
        first_mouse = false;
    }


    double xoffset = x - last_x;
    double yoffset = last_y - y;
    last_x = x;
    last_y = y;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
    front.y = static_cast<float>(sin(glm::radians(pitch)));
    front.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    camera.front = glm::normalize(front);
}




IndexedMesh generate_terrain(unsigned size, unsigned samples, float height, const ImageData& height_map)
{
    unsigned count = samples * samples;

    std::vector<Vertex> vertices(count);
    std::vector<GLuint> indices(6 * (samples - 1) * (samples - 1));

    int vertex_index = 0;
    for (unsigned z = 0; z < samples; ++z)
    {
        for (unsigned x = 0; x < samples; ++x)
        {
            float last = static_cast<GLfloat>(samples - 1);

            Vertex& vertex = vertices[vertex_index];

            unsigned row    = static_cast<unsigned>(lerp(0, static_cast<float>(height_map.height), static_cast<float>(z)/static_cast<float>(samples)));
            unsigned column = static_cast<unsigned>(lerp(0, static_cast<float>(height_map.width),  static_cast<float>(x)/static_cast<float>(samples)));


            glm::vec3& position = vertex.position;
            position.x = (x / last) * size;
            position.y = lerp(0, height, height_map.at(row, column) / 255.0f);
            position.z = (z / last) * size;

            glm::vec2& texture_coordinate = vertex.texture_coordinate;
            texture_coordinate.x = static_cast<GLfloat>(x) / last;
            texture_coordinate.y = static_cast<GLfloat>(z) / last;

            glm::vec3& normal = vertex.normal;
            normal.x = 0;
            normal.y = 1;
            normal.z = 0;

            ++vertex_index;
        }
    }


    int index_pointer = 0;
    for (unsigned z = 0; z < samples-1; ++z)
    {
        for (unsigned x = 0; x < samples-1; ++x)
        {
            GLuint top_left     = (z * samples) + x;
            GLuint top_right    = top_left + 1;
            GLuint bottom_left  = (z + 1) * samples + x;
            GLuint bottom_right = bottom_left + 1;

            indices[index_pointer++] = top_left;
            indices[index_pointer++] = bottom_left;
            indices[index_pointer++] = top_right;
            indices[index_pointer++] = top_right;
            indices[index_pointer++] = bottom_left;
            indices[index_pointer++] = bottom_right;
        }
    }

    return {vertices, indices};

}


int main()
{
    Window window = Window::create(800, 600, "LearnOpenGL");
    // glfwSetFramebufferSizeCallback(window.id, framebuffer_size_callback);
    glfwSetCursorPosCallback(window.id, mouse_callback);
    mouse_callback(window.id, last_x, last_y);
    // glfwSetScrollCallback(window.id, scroll_callback);
    glfwSetInputMode(window.id, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    glm::mat4 view = View(camera);
    glm::mat4 perspective = Perspective(camera);
    glm::mat4 model = glm::mat4();

    // SHADERS
    ShaderProgram object_shader = ShaderProgram::create(
        {ShaderProgram::vertex(read("../shaders/terrain.vertex.glsl")),
         ShaderProgram::fragment(read("../shaders/terrain.fragment.glsl"))}
    );
    ShaderProgram skybox_shader = ShaderProgram::create(
            {ShaderProgram::vertex(read("../shaders/skybox.vertex.glsl")),
             ShaderProgram::fragment(read("../shaders/skybox.fragment.glsl"))}
    );

    // TEXTURES
    GLuint blend_map_texture = load_texture(ImageData::create("../resources/images/blend_map.png"));
    GLuint grass_texture     = load_texture(ImageData::create("../resources/images/grassdirt-big.png"));
    GLuint mud_texture       = load_texture(ImageData::create("../resources/images/mud.png"));
    GLuint flowers_texture   = load_texture(ImageData::create("../resources/images/flowers.png"));
    GLuint path_texture      = load_texture(ImageData::create("../resources/images/path.png"));
    GLuint cube_map_texture  = load_3D_texture({
        "../resources/skybox/temp/right.tga",  "../resources/skybox/temp/left.tga", "../resources/skybox/temp/top.tga",
        "../resources/skybox/temp/bottom.tga", "../resources/skybox/temp/back.tga", "../resources/skybox/temp/front.tga"
    });


    // Terrain
    ImageData height_map = ImageData::create("../resources/heightmaps/heightmap.png", ImageData::GREY);
    IndexedMesh mesh = generate_terrain(1000, 256, 30.0f, height_map);
    IndexedModel terrain = IndexedModel::create(mesh.vertices, mesh.indices);

    Model skybox = create_skybox_cube();


    double delta_time = 0;
    double last_time  = 0;
    while (!glfwWindowShouldClose(window.id))
    {
        // TIME
        double current_frame = glfwGetTime();
        delta_time = current_frame - last_time;
        last_time = current_frame;


        // INPUT
        handle_input(window, camera);


        // UPDATE
        view = camera.view();
        velocity.y += GRAVITY * delta_time;
        camera.position += velocity;

        // position.x = (x / last) * size
        // x = (position.x / size) * last
        unsigned x = static_cast<unsigned>((camera.position.x / 1000.0f) * 255.0f);
        unsigned z = static_cast<unsigned>((camera.position.z / 1000.0f) * 255.0f);

        float height = 15.0f;
        if (0 <= x && x < 256 && 0 <= z && z < 256)
        {
            Vertex vertex = mesh.vertices[z * 256 + x];

            if (camera.position.y <= vertex.position.y + height)
            {
                on_ground = true;
                camera.position.y = vertex.position.y + height;
                velocity = glm::vec3{};
            }
            else
            {
                on_ground = false;
            }
        }
        else
        {
            // on_ground = true;
            camera.position.y = std::max(height, camera.position.y);
            velocity = glm::vec3{};
        }


        // RENDER
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


        // Object shader
        object_shader.enable();
        object_shader.load("view", view);
        object_shader.load("projection", perspective);
        object_shader.load("model",  model);
        object_shader.load("blend_map_texture", 0);
        object_shader.load("grass_texture", 1);
        object_shader.load("mud_texture", 2);
        object_shader.load("flowers_texture", 3);
        object_shader.load("path_texture", 4);
        object_shader.load_2D_textures({blend_map_texture, grass_texture, mud_texture, flowers_texture, path_texture});
        terrain.draw();


        // Skybox shader
        GLCALL(glDepthFunc(GL_LEQUAL));
        skybox_shader.enable();
        glm::mat4 skybox_view = glm::mat4(glm::mat3(view));
        skybox_shader.load("view", skybox_view);
        skybox_shader.load("projection", perspective);
        skybox_shader.load("skybox", 0);
        skybox_shader.load_3D_textures({cube_map_texture});
        skybox.draw();
        GLCALL(glDepthFunc(GL_LESS));


        // OTHER
        glfwSwapBuffers(window.id);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}

