#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <imgui.h>
#include <implementation/imgui_impl_glfw.h>
#include <implementation/imgui_impl_opengl3.h>

#include "gl_debug.h"
#include "camera.h"
#include "window.h"
#include "shader.h"
#include "utilities.h"
#include "model.h"

// Skyboxes: http://www.custommapmakers.org/skyboxes.php


bool  first_mouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float last_x = 800.0f / 2.0f;
float last_y = 600.0f / 2.0f;
float fov   =  45.0f;


// Camera
Camera camera;
float camera_speed = 4.0f;


double delta_time = 0.0;  // Time between current frame and last frame
double last_time = 0.0;  // Time of last frame


int program_selection = 0;
bool show_normals  = false;
bool show_mouse    = false;

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        show_mouse = !show_mouse;

        if (show_mouse)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // glfwSetWindowShouldClose(window, true);
    }

    auto speed = static_cast<float>(delta_time * camera_speed);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += speed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= speed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.position += glm::vec3(0.0f, 1.0f, 0.0f) * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera_speed = 10.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera_speed = 4.0f;

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) program_selection = 0;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) program_selection = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) program_selection = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) program_selection = 3;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) program_selection = 4;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) program_selection = 5;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) show_normals = !show_normals;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        GLCALL(glEnable(GL_MULTISAMPLE));
        std::cout << "Multisample enabled." << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        GLCALL(glDisable(GL_MULTISAMPLE));
        std::cout << "Multisample disabled." << std::endl;
    }

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

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.front = glm::normalize(front);
}


void scroll_callback(GLFWwindow* window, double xoffset, double y)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= y;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    GLCALL(glViewport(0, 0, width, height));
}


std::tuple<GLuint, GLuint> create_color_frame_buffer(unsigned width, unsigned height)
{
    GLuint fbo;
    GLCALL(glGenFramebuffers(1, &fbo));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    GLuint color_buffer;
    GLCALL(glGenTextures(1, &color_buffer));
    GLCALL(glBindTexture(GL_TEXTURE_2D, color_buffer));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0));

    GLuint rbo;
    GLCALL(glGenRenderbuffers(1, &rbo));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));

    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo));

    GLCALL(GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Incomplete framebuffer!");
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    return {fbo, color_buffer};
}


std::tuple<GLuint, GLuint> create_multi_sampled_color_frame_buffer(unsigned width, unsigned height, unsigned samples = 4)
{
    GLuint fbo;
    GLCALL(glGenFramebuffers(1, &fbo));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    GLuint color_buffer;
    GLCALL(glGenTextures(1, &color_buffer));
    GLCALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_buffer));
    GLCALL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE));

    GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, color_buffer, 0));

    GLuint rbo;
    GLCALL(glGenRenderbuffers(1, &rbo));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
    GLCALL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height));

    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo));

    GLCALL(GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Incomplete framebuffer!");
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    return {fbo, color_buffer};
}



int main()
{
    Window window = Window::create(800, 600, "LearnOpenGL");
    glfwSetFramebufferSizeCallback(window.id, framebuffer_size_callback);
    glfwSetCursorPosCallback(window.id, mouse_callback);
    glfwSetScrollCallback(window.id, scroll_callback);
    glfwSetInputMode(window.id, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    //
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); // (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfw_InitForOpenGL(window.id, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();  // Setup style



    camera.position = glm::vec3(0.0f, 3.0f, 10.0f);
    camera.aspect_ratio = static_cast<float>(window.width) / static_cast<float>(window.height);

    // Positions
    glm::vec3 object_position(0.0f, 0.40f, 0.0f);

    glm::vec3 cube_positions[] = {
            glm::vec3( 5.0f, 0.5f, 0.0f),
            glm::vec3( 2.0f, 0.5f, 6.0f),
            glm::vec3(-5.5f, 0.5f, 2.5f),
            glm::vec3(-2.8f, 0.5f, 8.3f),
            glm::vec3( 6.4f, 0.5f, 8.5f),
    };

    glm::vec3 point_light_positions[] = {
            glm::vec3( 0.7f, 3.2f,  2.0f),
            glm::vec3( 2.3f, 3.3f, -4.0f),
            glm::vec3(-4.0f, 3.0f, 0.0f),
            glm::vec3( 2.0f, 3.5f, 5.0f)
    };

    glm::vec3 directional_light_direction = glm::vec3(-0.2f, -1.0f, -0.3f);

    glm::vec3 point_light_colors[] = {
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
    };

    std::vector<glm::vec3> vegetation;
    vegetation.emplace_back(-1.5f, 0.5f, 3.48f);
    vegetation.emplace_back( 1.5f, 0.5f, 3.51f);
    vegetation.emplace_back( 0.0f, 0.5f, 4.7f);
    vegetation.emplace_back(-0.3f, 0.5f, 3.3f);
    vegetation.emplace_back( 2.0f, 0.5f, 4.6f);


    // Transformation
    glm::mat4 model = {};
    glm::mat4 view = camera.view();
    glm::mat4 perspective = camera.perspective();


    // Models
    // GLuint cube_vao   = create_cube();
    // GLuint quad_vao   = create_quad();
    // GLuint skybox_vao = create_skybox_cube();
    Model cube = Model::cube();
    Model quad = Model::quad();


    // Texturing
    GLuint texture1 = load_texture("../resources/images/container.png");
    GLuint texture2 = load_texture("../resources/images/container_specular.png");
    GLuint texture_house_diffuse = load_texture("../resources/images/house_diffuse.png", true);
    GLuint vegetation_texture = load_texture("../resources/images/grass.png", true);
    GLuint window_texture = load_texture("../resources/images/window.png");

    std::vector<std::string> cube_textures = {
//            "../resources/skybox/temp/right.tga",
//            "../resources/skybox/temp/left.tga",
//            "../resources/skybox/temp/top.tga",
//            "../resources/skybox/temp/bottom.tga",
//            "../resources/skybox/temp/back.tga",
//            "../resources/skybox/temp/front.tga"

            "../resources/skybox/ruins/right.tga",
            "../resources/skybox/ruins/left.tga",
            "../resources/skybox/ruins/top.tga",
            "../resources/skybox/ruins/bottom.tga",
            "../resources/skybox/ruins/back.tga",
            "../resources/skybox/ruins/front.tga"

//
//            "../resources/skybox/right.jpg",
//            "../resources/skybox/left.jpg",
//            "../resources/skybox/top.jpg",
//            "../resources/skybox/bottom.jpg",
//            "../resources/skybox/back.jpg",
//            "../resources/skybox/front.jpg"
//
//            "../resources/images/container.png",
//            "../resources/images/container.png",
//            "../resources/images/container.png",
//            "../resources/images/container.png",
//            "../resources/images/container.png",
//            "../resources/images/container.png"
    };
    GLuint cube_map_texture = load_3D_texture(cube_textures);


    // SHADERS
    ShaderProgram object_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/lightning.vertex.glsl")), Shader::fragment(read("../shaders/lightning.fragment.glsl"))}
    );
    ShaderProgram light_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/lightning.vertex.glsl")),  Shader::fragment(read("../shaders/lamp.fragment.glsl"))}
    );
    ShaderProgram trans_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/quad.vertex.glsl")),       Shader::fragment(read("../shaders/quad.fragment.glsl"))}
    );
    ShaderProgram skybox_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/skybox.vertex.glsl")),     Shader::fragment(read("../shaders/skybox.fragment.glsl"))}
    );
    ShaderProgram reflective_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/reflective.vertex.glsl")), Shader::fragment(read("../shaders/reflective.fragment.glsl"))}
    );
    ShaderProgram refractive_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/refractive.vertex.glsl")), Shader::fragment(read("../shaders/refractive.fragment.glsl"))}
    );
    ShaderProgram instanced_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/instanced.vertex.glsl")),  Shader::fragment(read("../shaders/lightning.fragment.glsl"))}
    );

    // Shadow
    ShaderProgram shadow_program = ShaderProgram::create({Shader::vertex(read("../shaders/shadow.vertex.glsl")), Shader::fragment(read("../shaders/shadow.fragment.glsl"))});



    // Debug
    ShaderProgram normal_debug_shader = ShaderProgram::create({
        Shader::vertex(read("../shaders/normal-debug.vertex.glsl")),
        Shader::fragment(read("../shaders/normal-debug.fragment.glsl")),
        Shader::geometry(read("../shaders/normal-debug.geometry.glsl"))
    });

    // Post-processing
    ShaderProgram view_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/post-processing/screen.vertex.glsl")), Shader::fragment(read("../shaders/post-processing/view.fragment.glsl"))}
    );
    ShaderProgram greyscale_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/post-processing/screen.vertex.glsl")), Shader::fragment(read("../shaders/post-processing/grey-scale.fragment.glsl"))}
    );
    ShaderProgram sharpen_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/post-processing/screen.vertex.glsl")), Shader::fragment(read("../shaders/post-processing/sharpen.fragment.glsl"))}
    );
    ShaderProgram inversion_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/post-processing/screen.vertex.glsl")), Shader::fragment(read("../shaders/post-processing/inversion.fragment.glsl"))}
    );
    ShaderProgram edge_detection_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/post-processing/screen.vertex.glsl")), Shader::fragment(read("../shaders/post-processing/edge-detection.fragment.glsl"))}
    );
    ShaderProgram blur_shader = ShaderProgram::create(
        {Shader::vertex(read("../shaders/post-processing/screen.vertex.glsl")), Shader::fragment(read("../shaders/post-processing/blur.fragment.glsl"))}
    );

    ShaderProgram post_processing_shaders[] = {
            view_shader,
            greyscale_shader,
            sharpen_shader,
            inversion_shader,
            edge_detection_shader,
            blur_shader
    };

    ShaderProgram::create_shared_uniform_block("ViewPerspective", sizeof(GLfloat) * 4 * 4 * 2);


    object_shader.enable();
    object_shader.add_shared("ViewPerspective");
    object_shader.load("model", model);
    object_shader.load("camera_position", camera.position);
    object_shader.load("shadow_map", 2);
    object_shader.load("material.diffuse",   0);
    object_shader.load("material.specular",  1);
    object_shader.load("material.shininess", 32.0f);
    object_shader.load("directional_light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    object_shader.load("directional_light.ambient",   glm::vec3(0.1f,   0.1f,  0.1f));
    object_shader.load("directional_light.diffuse",   glm::vec3(0.3f,   0.3f,  0.3f));
    object_shader.load("directional_light.specular",  glm::vec3(0.8f,   0.8,   0.8f));

    for (unsigned int i = 0; i < 4; ++i)
    {
        object_shader.load("point_light", i, "position" , point_light_positions[i]);
        object_shader.load("point_light", i, "constant" , 1.0f);
        object_shader.load("point_light", i, "linear"   , 0.09f);
        object_shader.load("point_light", i, "quadratic", 0.032f);
        object_shader.load("point_light", i, "ambient"  , glm::vec3(0.0f,  0.0f,  0.0f));
        object_shader.load("point_light", i, "diffuse"  , point_light_colors[i]);
        object_shader.load("point_light", i, "specular" , point_light_colors[i]);
    }


    light_shader.enable();
    light_shader.add_shared("ViewPerspective");
    light_shader.load("model", model);
    light_shader.load("color", glm::vec3(0.5f, 0.5f, 0.5f));


    trans_shader.enable();
    trans_shader.add_shared("ViewPerspective");
    trans_shader.load("model", model);
    trans_shader.load("image", 0);


    skybox_shader.enable();
    skybox_shader.load("skybox_view", view);
    skybox_shader.load("perspective", perspective);
    skybox_shader.load("skybox", 0);


    reflective_shader.enable();
    reflective_shader.add_shared("ViewPerspective");
    reflective_shader.load("skybox", 0);


    refractive_shader.enable();
    refractive_shader.add_shared("ViewPerspective");
    refractive_shader.load("skybox", 0);


    normal_debug_shader.enable();
    normal_debug_shader.add_shared("ViewPerspective");
    normal_debug_shader.load("model", model);


    instanced_shader.enable();
    instanced_shader.add_shared("ViewPerspective");
    instanced_shader.load("camera_position", camera.position);
    instanced_shader.load("shadow_map", 2);
    instanced_shader.load("material.diffuse",   0);
    instanced_shader.load("material.specular",  1);
    instanced_shader.load("material.shininess", 32.0f);
    instanced_shader.load("directional_light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    instanced_shader.load("directional_light.ambient",   glm::vec3(0.1f,   0.1f,  0.1f));
    instanced_shader.load("directional_light.diffuse",   glm::vec3(0.3f,   0.3f,  0.3f));
    instanced_shader.load("directional_light.specular",  glm::vec3(0.8f,   0.8,   0.8f));

    for (unsigned int i = 0; i < 4; ++i)
    {
        instanced_shader.load("point_light", i, "position" , point_light_positions[i]);
        instanced_shader.load("point_light", i, "constant" , 1.0f);
        instanced_shader.load("point_light", i, "linear"   , 0.09f);
        instanced_shader.load("point_light", i, "quadratic", 0.032f);
        instanced_shader.load("point_light", i, "ambient"  , glm::vec3(0.0f,  0.0f,  0.0f));
        instanced_shader.load("point_light", i, "diffuse"  , point_light_colors[i]);
        instanced_shader.load("point_light", i, "specular" , point_light_colors[i]);
    }


    std::vector<TexturedModel> house = load_obj_file("../resources/models/ship.obj");
    // TexturedModel house = load_obj_file("../resources/models/ship.obj");



    // POST PROCESING FRAMEBUFFERS
    GLuint fbo;
    GLuint color_buffer;
    std::tie(fbo, color_buffer) = create_color_frame_buffer(window.width, window.height);
    // std::tie(fbo, color_buffer) = create_multi_sampled_color_frame_buffer(window.width, window.height);


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



    // INSTANCE RENDERING
    unsigned int amount = 500;
    glm::mat4* matrices = new glm::mat4[amount];
    srand(glfwGetTime());
    float radius = 30.0;
    float offset = 25.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 instance_model;
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to window.width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        instance_model = glm::translate(instance_model, glm::vec3(x, y, z));

        // 2. scale
        float scale = (rand() % 95) / 100.0f + 0.05;
        instance_model = glm::scale(instance_model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        instance_model = glm::rotate(instance_model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        matrices[i] = instance_model;
    }

    GLCALL(glBindVertexArray(cube.id));

    GLuint buffer;
    GLCALL(glGenBuffers(1, &buffer));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &matrices[0], GL_STATIC_DRAW));

    // set attribute pointers for matrix (4 times vec4)
    GLCALL(glEnableVertexAttribArray(3));
    GLCALL(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0));
    GLCALL(glEnableVertexAttribArray(4));
    GLCALL(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1* sizeof(glm::vec4))));
    GLCALL(glEnableVertexAttribArray(5));
    GLCALL(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))));
    GLCALL(glEnableVertexAttribArray(6));
    GLCALL(glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))));

    GLCALL(glVertexAttribDivisor(3, 1));
    GLCALL(glVertexAttribDivisor(4, 1));
    GLCALL(glVertexAttribDivisor(5, 1));
    GLCALL(glVertexAttribDivisor(6, 1));

    GLCALL(glBindVertexArray(0));





//    GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCALL(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO));

//    GLCALL(glEnable(GL_CULL_FACE));
//    GLCALL(glCullFace(GL_BACK));
    GLCALL(glFrontFace(GL_CCW));



    // MAIN GAME LOOP
    while (!glfwWindowShouldClose(window.id))
    {
        // Handle time
        double current_frame = glfwGetTime();
        delta_time = current_frame - last_time;
        last_time = current_frame;

        // Input
        process_input(window.id);

        // Update
        view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
        perspective = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        for (auto& position : point_light_positions)
        {
            position.x += sin(glfwGetTime()) * 0.05;
        }
        directional_light_direction.z = sin(glfwGetTime()) * 0.05f;


        glm::mat4 light_perspective = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);
        glm::mat4 light_view = glm::lookAt(glm::vec3() - directional_light_direction * 10.0f, glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 light_perspective_view = light_perspective * light_view;


        glm::mat4 vp[] = {view, perspective};
        ShaderProgram::load_shared_uniform_block("ViewPerspective", vp, sizeof(GLfloat) * 4 * 4 * 2);


        // RENDER
        // Global
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


        // Shadow
        GLCALL(glCullFace(GL_FRONT));

        GLCALL(glViewport(0, 0, SHADOW_WIDTH_RESOLUTION, SHADOW_HEIGHT_RESOLUTION));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo));
        GLCALL(glClear(GL_DEPTH_BUFFER_BIT));

        shadow_program.enable();
        shadow_program.load("light_perspective_view", light_perspective_view);

        cube.enable();
        for (auto& position : cube_positions)
        {
            model = {};
            model = glm::translate(model, position);
            shadow_program.load("model", model);
            cube.draw();
        }


        if (program_selection == 0)
        {
            GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));    // Render on-screen
        }
        else
        {
            GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));  // Render off-screen
        }
        GLCALL(glCullFace(GL_BACK));
        GLCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
        GLCALL(glViewport(0, 0, window.width, window.height));


        // Debug
        if (show_normals)
        {
            // Cubes with normals
            normal_debug_shader.enable();
            normal_debug_shader.load_2D_textures({texture1, texture2});

            cube.enable();
            for (auto& position : cube_positions)
            {
                model = {};
                model = glm::translate(model, position);
                normal_debug_shader.load("model", model);
                cube.draw();
            }

            GLCALL(glBindVertexArray(cube.id));
            GLCALL(glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount));
        }


        // Instanced cubes
        instanced_shader.enable();
        instanced_shader.load_2D_textures({texture1, texture2, shadow_map});
        instanced_shader.load("camera_position", camera.position);
        instanced_shader.load("directional_light.direction", directional_light_direction);
        instanced_shader.load("light_perspective_view", light_perspective_view);
        for (unsigned int i = 0; i < 4; ++i)
            instanced_shader.load("point_light", i, "position", point_light_positions[i]);
        GLCALL(glBindVertexArray(cube.id));
        GLCALL(glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount));


        // Cubes
        object_shader.enable();
        object_shader.load_2D_textures({texture1, texture2, shadow_map});
        object_shader.load("camera_position", camera.position);
        object_shader.load("directional_light.direction", directional_light_direction);
        object_shader.load("light_perspective_view", light_perspective_view);
        for (unsigned int i = 0; i < 4; ++i)
            object_shader.load("point_light", i, "position", point_light_positions[i]);


        cube.enable();
        for (auto& position : cube_positions)
        {
            model = {};
            model = glm::translate(model, position);
            object_shader.load("model", model);
            cube.draw();
        }


        // Model
        model = {};
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        model = glm::translate(model, object_position);
        // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        object_shader.load_2D_textures({texture_house_diffuse, texture_house_diffuse});
        object_shader.load("model", model);

        for (auto& mesh : house)
            mesh.draw();




        // Reflective cubes
        reflective_shader.enable();
        reflective_shader.load("camera_position", camera.position);

        cube.enable();
        for (auto& position : cube_positions)
        {
            model = {};
            model = glm::translate(model, position + glm::vec3(2.0f, 8.0f, -2.0f));
            reflective_shader.load("model", model);
            cube.draw();
        }



        // Refractive cubes
        refractive_shader.enable();
        refractive_shader.load("camera_position", camera.position);

        cube.enable();
        for (auto& position : cube_positions)
        {
            model = {};
            model = glm::translate(model, position + glm::vec3(-2.0f, 16.0f, 2.0f));
            refractive_shader.load("model", model);
            cube.draw();
        }



        // Lights
        light_shader.enable();

        cube.enable();
        int x = 0;
        for (auto& position : point_light_positions)
        {
            glm::vec3 color = point_light_colors[x++];
            model = {};
            model = glm::translate(model, position);
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            light_shader.load("model", model);
            light_shader.load("color", color);
            cube.draw();
        }




        // Transparent's
        // Needs to sort for transparent objects.
        std::sort(vegetation.begin(), vegetation.end(),
            [&](const glm::vec3& position1, const glm::vec3& position2)
            {
                return ::glm::length(::camera.position - position1) >= ::glm::length(::camera.position - position2);
            }
        );

        trans_shader.enable();
        trans_shader.load_2D_textures({window_texture});  // vegetation_texture

        quad.enable();
        for (auto& position : vegetation)
        {
            model = {};
            model = glm::translate(model, position);
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
            trans_shader.load("model", model);
            quad.draw();
        }


        // Skybox - Last to optimize
        GLCALL(glDepthFunc(GL_LEQUAL));
        skybox_shader.enable();

        glm::mat4 skybox_view = glm::mat4(glm::mat3(view));
        skybox_shader.load("skybox_view", skybox_view);
        skybox_shader.load("perspective", perspective);
        skybox_shader.load("skybox", 0);

        skybox_shader.load_3D_textures({cube_map_texture});
        cube.enable();
        cube.draw();

        GLCALL(glDepthFunc(GL_LESS));



        // GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
        // GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        // GLCALL(glBlitFramebuffer(0, 0, window.width, window.height, 0, 0, window.width, window.height, GL_COLOR_BUFFER_BIT, GL_NEAREST));

        // Render off-screen buffer
        if (program_selection != 0)
        {
            GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));  // Render on-screen

            GLCALL(glDisable(GL_DEPTH_TEST));
            GLCALL(glClearColor(0.0f, 1.0f, 0.0f, 1.0f));
            GLCALL(glClear(GL_COLOR_BUFFER_BIT));

            ShaderProgram prost_processing = post_processing_shaders[program_selection];
            prost_processing.enable();
            prost_processing.load_2D_textures({color_buffer});
            quad.enable();
            quad.draw();
        }


        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat3("position", &object_position.x, -10.0f, 10.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



        glfwSwapBuffers(window.id);
        glfwPollEvents();
    }

    GLCALL(glDeleteVertexArrays(1, &cube.id));
//    GLCALL(glDeleteFramebuffers(1, &fbo));


    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
