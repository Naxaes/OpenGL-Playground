#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_debug.h"
#include "camera.h"
#include "utilities.h"
#include "window.h"
#include "texture.h"
#include "shader.h"
#include "model.h"

bool  first_mouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float last_x = 800.0f / 2.0f;
float last_y = 600.0f / 2.0f;


// Camera
Camera camera;
float camera_speed = 4.0f;


double delta_time = 0.0;  // Time between current frame and last frame
double last_time = 0.0;  // Time of last frame

bool debug = false;
bool show_normals = false;


glm::mat4 ModelMatrix(glm::vec3 position, glm::vec3 rotation = {0.0f, 0.0f, 0.0f}, glm::vec3 scale = {1.0f, 1.0f, 1.0f})
{
    glm::mat4 model(0);
    model = glm::scale(glm::rotate(glm::translate(model, position), glm::radians(360.0f), rotation), scale);
    return model;
}



void ProcessInput(Window window)
{
    if (glfwGetKey(window.id, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.id, true);

    auto speed = static_cast<float>(delta_time * camera_speed);
    if (glfwGetKey(window.id, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += speed * camera.front;
    if (glfwGetKey(window.id, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= speed * camera.front;
    if (glfwGetKey(window.id, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * speed;
    if (glfwGetKey(window.id, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * speed;
    if (glfwGetKey(window.id, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.position += glm::vec3(0.0f, 1.0f, 0.0f) * speed;
    if (glfwGetKey(window.id, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
    if (glfwGetKey(window.id, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera_speed = 10.0f;
    if (glfwGetKey(window.id, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera_speed = 4.0f;

    if (glfwGetKey(window.id, GLFW_KEY_P) == GLFW_PRESS)
        debug = true;
    if (glfwGetKey(window.id, GLFW_KEY_P) == GLFW_RELEASE)
        debug = false;

    if (glfwGetKey(window.id, GLFW_KEY_N) == GLFW_PRESS) show_normals = !show_normals;

}


void mouse_callback(GLFWwindow* window, double x, double y)
{
    if (first_mouse)
    {
        last_x = x;
        last_y = y;
        first_mouse = false;
    }

    float xoffset = x - last_x;
    float yoffset = last_y - y;
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
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.front = glm::normalize(front);
}


void scroll_callback(GLFWwindow* window, double xoffset, double y)
{
    if (camera.fov >= 1.0f && camera.fov <= 45.0f)
        camera.fov -= y;
    if (camera.fov <= 1.0f)
        camera.fov = 1.0f;
    if (camera.fov >= 45.0f)
        camera.fov = 45.0f;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    GLCALL(glViewport(0, 0, width, height));
}


// Model LoadModelFile(const std::string& path)
// {
//     std::ifstream file(path);
//     if (!file.is_open())
//         throw std::runtime_error("Couldn't open file " + path + ".");
//
//
//     auto GetCount = [](std::ifstream& file) -> int {
//         int count;
//         file >> count;
//         file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Skip to next line
//         return count;
//     };
//
//     Model model;
//
//     std::string word;
//     while (file >> word)
//     {
//         if (word == "#NAME")
//         {
//             int count = GetCount(file);
//             model.name.reserve(count);
//             std::getline(file, model.name);
//         }
//         else if (word == "#VERTICES")
//         {
//             int count = GetCount(file);
//             model.vertices.reserve(count);
//             int index = 0;
//
//             std::string source;
//             char delimiter = ' ';
//
//             std::getline(file, source);
//
//             unsigned long start = 0U;
//             unsigned long end   = source.find(delimiter);
//
//             while (end != std::string::npos)
//             {
//                 model.vertices.emplace_back(std::stof(source.substr(start, end - start)));
//                 start = end + 1;
//                 end   = source.find(delimiter, start);
//             }
//
//             model.vertices.emplace_back(std::stof(source.substr(start, end - start)));
//
//         }
//         else if (word == "#INDICES")
//         {
//             int count = GetCount(file);
//             model.indices.reserve(count);
//             int index = 0;
//
//             std::string source;
//             char delimiter = ' ';
//
//             std::getline(file, source);
//
//             unsigned long start = 0U;
//             unsigned long end   = source.find(delimiter);
//
//             while (end != std::string::npos)
//             {
//                 model.indices.emplace_back(std::stof(source.substr(start, end - start)));
//                 start = end + 1;
//                 end   = source.find(delimiter, start);
//             }
//
//             model.indices.emplace_back(std::stof(source.substr(start, end - start)));
//         }
//         else
//         {
//             throw std::runtime_error("Unexpected token " + word + ".");
//         }
//     }
//
//     return model;
// }


// GLuint LoadModel(const Model& model)
// {
//     GLuint vao, vbo, ebo;
//     GLCALL(glGenVertexArrays(1, &vao));
//     GLCALL(glGenBuffers(1, &vbo));
//     GLCALL(glGenBuffers(1, &ebo));
//
//     GLCALL(glBindVertexArray(vao));
//
//     GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
//     GLCALL(glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * 4, &model.vertices[0], GL_STATIC_DRAW));
//
//     GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
//     GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * 4, &model.indices[0], GL_STATIC_DRAW));
//
//     GLCALL(glEnableVertexAttribArray(0));
//     GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0));
//     GLCALL(glEnableVertexAttribArray(1));
//     GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))));
//     GLCALL(glEnableVertexAttribArray(2));
//     GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))));
//
//     GLCALL(glBindVertexArray(0));
//
//     return vao;
// }



int main()
{
    stbi_set_flip_vertically_on_load(true);

    Window window = Window::create(800, 600, "LearnOpenGL");
    glfwSetFramebufferSizeCallback(window.id, framebuffer_size_callback);
    glfwSetCursorPosCallback(window.id, mouse_callback);
    glfwSetScrollCallback(window.id, scroll_callback);
    glfwSetInputMode(window.id, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    camera.position = glm::vec3(0.0f, 4.5f, 5.0f);
    camera.aspect_ratio = static_cast<float>(window.width) / static_cast<float>(window.height);


    // TRANSFORMATION
    glm::mat4 model(0);
    glm::mat4 view = View(camera);
    glm::mat4 projection = Perspective(camera);



    glm::vec3 cube_positions[] = {
        glm::vec3(0.0f, 2.3f, 0.0f),
    };
    glm::vec3 quad_positions[] = {
        glm::vec3(0.0f, -0.5f, 0.0f),
    };

    glm::vec3 light_position = glm::vec3(0.0f, 5.0f, 0.0f);

    // MODELS
    Model cube = Model::cube();
    Model quad = Model::quad();


    // TEXTURES
    GLuint diffuse_texture = load_texture(ImageData::create("../resources/images/container.png"));
    GLuint floor_texture   = load_texture(ImageData::create("../resources/images/grassdirt-big.png"));


    // SHADERS
    ShaderProgram object_program       = ShaderProgram::create({ShaderProgram::vertex(read("../shaders/lightshadowv.glsl")), ShaderProgram::fragment(read("../shaders/lightshadowf.glsl"))});
    ShaderProgram shadow_program       = ShaderProgram::create({ShaderProgram::vertex(read("../shaders/shadowv.glsl")), ShaderProgram::fragment(read("../shaders/shadowf.glsl"))});
    ShaderProgram depth_debug_program  = ShaderProgram::create({ShaderProgram::vertex(read("../shaders/depthbufferv.glsl")), ShaderProgram::fragment(read("../shaders/depthbufferf.glsl"))});
    ShaderProgram normal_debug_program = ShaderProgram::create({ShaderProgram::vertex(read("../shaders/normaldebugv.glsl")), ShaderProgram::fragment(read("../shaders/normaldebugf.glsl")), ShaderProgram::geometry(read("../shaders/normaldebugg.glsl"))});


    // SHADOW MAP
    GLuint shadow_map_fbo;
    GLCALL(glGenFramebuffers(1, &shadow_map_fbo));

    // Changing the resolution provides better looking shadows.
    constexpr unsigned int SHADOW_WIDTH_RESOLUTION = 1024, SHADOW_HEIGHT_RESOLUTION = 1024;
    GLuint shadow_map;
    GLCALL(glGenTextures(1, &shadow_map));
    GLCALL(glBindTexture(GL_TEXTURE_2D, shadow_map));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH_RESOLUTION, SHADOW_HEIGHT_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo));
    GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map, 0));
    GLCALL(glDrawBuffer(GL_NONE));
    GLCALL(glReadBuffer(GL_NONE));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));


    // MAIN LOOP
    while (!glfwWindowShouldClose(window.id))
    {
        // TIME
        auto current_time = static_cast<float>(glfwGetTime());
        delta_time = current_time - last_time;
        last_time  = current_time;


        // INPUT
        ProcessInput(window);


        // UPDATE
        projection = Perspective(camera);
        view = View(camera);

        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);
        glm::mat4 light_view = glm::lookAt(light_position, cube_positions[0], glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 light_projection_view = light_projection * light_view;

        light_position.z = sin(glfwGetTime()) * 3;
        light_position.x = cos(glfwGetTime()) * 3;
        quad_positions[0].z = sin(glfwGetTime());


        // RENDER
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glClearColor(0, 0, 0, 1.0f));


        // Shadows
        GLCALL(glCullFace(GL_FRONT));

        GLCALL(glViewport(0, 0, SHADOW_WIDTH_RESOLUTION, SHADOW_HEIGHT_RESOLUTION));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo));
        GLCALL(glClear(GL_DEPTH_BUFFER_BIT));

        shadow_program.enable();
        shadow_program.load("light_projection_view", light_projection_view);

        for (auto& position : cube_positions)
        {
            model = {};
            model = glm::translate(model, position);
            shadow_program.load("model", model);
            cube.draw();
        }
        {
            model = {};
            model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::translate(model, quad_positions[0]);
            shadow_program.load("model", model);
            quad.draw();
        }


        // Cubes
        GLCALL(glCullFace(GL_BACK));

        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

        object_program.enable();
        object_program.load("view", view);
        object_program.load("projection", projection);
        object_program.load("light_projection_view", light_projection_view);
        object_program.load("light_position", light_position);
        object_program.load("camera_position", camera.position);
        object_program.load("image", 0);
        object_program.load("shadow_map", 1);
        object_program.load_2D_textures({diffuse_texture, shadow_map});

        for (auto& position : cube_positions)
        {
            model = {};
            model = glm::translate(model, position);
            object_program.load("model", model);
            cube.draw();
        }
        {
            model = {};
            model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::translate(model, quad_positions[0]);
            object_program.load("model", model);
            object_program.load_2D_textures({floor_texture});
            quad.draw();
        }
        {  // Light
            model = {};
            model = glm::translate(model, light_position);
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            object_program.load("model", model);
            cube.draw();
        }


        // Normals
        if (show_normals) {
            normal_debug_program.enable();
            normal_debug_program.load("view", view);
            normal_debug_program.load("projection", projection);

            model = {};
            model = glm::translate(model, cube_positions[0]);
            normal_debug_program.load("model", model);
            cube.draw();

            model = {};
            model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::translate(model, quad_positions[0]);
            normal_debug_program.load("model", model);
            quad.draw();
        }


        if (debug)
        {
            GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            GLCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

            depth_debug_program.enable();
            depth_debug_program.load("depth_map", 0);
            depth_debug_program.load_2D_textures({shadow_map});

            quad.draw();
        }


        // OTHER
        glfwSwapBuffers(window.id);
        glfwPollEvents();

        // Sleep to save power.
//        auto x = static_cast<float>(glfwGetTime()) - current_time;
//        if (x < 1.0f / 30.0f)
//            std::this_thread::sleep_for(std::chrono::seconds(x));

    }


    glfwTerminate();
    return 0;
}