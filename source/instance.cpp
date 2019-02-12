#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

#define  STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "gl_debug.h"


// Skyboxes: http://www.custommapmakers.org/skyboxes.php

GLuint SHADER_LAYOUT_POSITION = 0;
GLuint SHADER_LAYOUT_NORMAL = 1;
GLuint SHADER_LAYOUT_TEXTURE_COORDINATE = 2;
//GLuint SHADER_LAYOUT_INSTANCED_MAT4x4 = 3;


constexpr unsigned int WIDTH  = 800;
constexpr unsigned int HEIGHT = 600;


bool  first_mouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float last_x = 800.0f / 2.0f;
float last_y = 600.0f / 2.0f;
float fov   =  45.0f;


// Camera
//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_position = glm::vec3(0.0f, 2.0f, 10.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f,  0.0f);
float camera_speed = 4.0f;


double delta_time = 0.0;  // Time between current frame and last frame
double last_time = 0.0;  // Time of last frame


struct ShaderProgram2
{
    GLuint id;
    std::unordered_map<std::string, GLuint> location;
};


void CacheUniforms(ShaderProgram2& program, const std::vector<std::string>& uniforms)
{
    GLCALL(glUseProgram(program.id));
    for (const std::string& uniform : uniforms)
    {
        GLCALL(program.location[uniform] = glGetUniformLocation(program.id, uniform.c_str()));
    }
}


void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    auto speed = static_cast<float>(delta_time * camera_speed);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_position += speed * camera_front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_position -= speed * camera_front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera_position += glm::vec3(0.0f, 1.0f, 0.0f) * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera_position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera_speed = 10.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera_speed = 4.0f;

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
    camera_front = glm::normalize(front);
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


std::string read(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file " + path + ".");

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


GLuint create_program(
        const std::string& vertex_file_path,
        const std::string& fragment_file_path,
        const std::string& geometry_file_path = ""
)
{
    // VERTEX SHADER
    std::string temp = read(vertex_file_path);
    const char* vertex_source = temp.c_str();
    GLuint vertex_shader;
    GLCALL(vertex_shader = glCreateShader(GL_VERTEX_SHADER));
    GLCALL(glShaderSource(vertex_shader, 1, &vertex_source, NULL));
    GLCALL(glCompileShader(vertex_shader));

    constexpr unsigned size = 512;
    int  success;
    char info[size];
    GLCALL(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success));
    if (!success)
    {
        GLCALL(glGetShaderInfoLog(vertex_shader, size, NULL, info));
        throw std::runtime_error(std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") + info + "\n\n" + vertex_source);
    }


    // GEOMETRY SHADER
    GLuint geometry_shader = 0;
    std::string temp3 = "";
    if (!geometry_file_path.empty())
    {
        temp3 = read(geometry_file_path);
        const char* geometry_source = temp3.c_str();
        GLCALL(geometry_shader = glCreateShader(GL_GEOMETRY_SHADER));
        GLCALL(glShaderSource(geometry_shader, 1, &geometry_source, NULL));
        GLCALL(glCompileShader(geometry_shader));

        GLCALL(glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            GLCALL(glGetShaderInfoLog(geometry_shader, size, NULL, info));
            throw std::runtime_error(std::string("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n") + info + "\n\n" + geometry_source);
        }
    }


    // FRAGMENT SHADER
    std::string temp2 = read(fragment_file_path);
    const char* fragment_source = temp2.c_str();
    GLuint fragment_shader;
    GLCALL(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
    GLCALL(glShaderSource(fragment_shader, 1, &fragment_source, NULL));
    GLCALL(glCompileShader(fragment_shader));

    GLCALL(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success));
    if (!success)
    {
        GLCALL(glGetShaderInfoLog(fragment_shader, size, NULL, info));
        throw std::runtime_error(std::string("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n") + info + "\n\n" + fragment_source);
    }

    // SHADER PROGRAM
    GLuint program;
    GLCALL(program = glCreateProgram());
    GLCALL(glAttachShader(program, vertex_shader));
    if (!geometry_file_path.empty()) { GLCALL(glAttachShader(program, geometry_shader)); }
    GLCALL(glAttachShader(program, fragment_shader));
    GLCALL(glLinkProgram(program));

    GLCALL(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success)
    {
        GLCALL(glGetProgramInfoLog(program, 512, NULL, info));
        throw std::runtime_error(
                std::string("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n") + info +
                "\nVERTEX SOURCE:\n"   + vertex_source +
                "\nGEOMETRY SOURCE:\n" + temp3 +
                "\nFRAGMENT SOURCE:\n" + fragment_source
        );

    }

    GLCALL(glDeleteShader(vertex_shader));
    if (!geometry_file_path.empty()) { GLCALL(glDeleteShader(geometry_shader)); }
    GLCALL(glDeleteShader(fragment_shader));

    return program;
}


GLuint load_texture(const std::string& path, bool flip = false)
{
    stbi_set_flip_vertically_on_load(flip);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data)
        throw std::runtime_error("Couldn't load texture " + path + ".");

    GLuint texture;
    GLCALL(glGenTextures(1, &texture));
    GLCALL(glBindTexture(GL_TEXTURE_2D, texture));

    GLCALL(glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER, GL_LINEAR));
//    GLfloat border_color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
//    GLCALL(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color));
//    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
//    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
    stbi_image_free(data);

    return texture;
}


GLuint CreateCubeMapTexture(std::vector<std::string> faces)
{
    GLuint texture;
    GLCALL(glGenTextures(1, &texture));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));

    int width, height, channels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        if (data)
        {
            GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            ));
            stbi_image_free(data);
        }
        else
        {
            stbi_image_free(data);
            throw std::runtime_error("Couldn't load texture at path " + faces[i] + ".");
        }
    }
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    return texture;
}


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinate;
};

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;
};

struct Mesh
{
    GLuint vao;
    std::vector<Vertex>   vertices;
    std::vector<unsigned> indices;
    std::vector<Texture>  textures;
};

GLuint create_mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices)
{
    GLuint vao, vbo, ebo;

    GLCALL(glGenVertexArrays(1, &vao));
    GLCALL(glGenBuffers(1, &vbo));
    GLCALL(glGenBuffers(1, &ebo));

    GLCALL(glBindVertexArray(vao));

    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    GLCALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));

    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW));

    // set the vertex attribute pointers
    // vertex Positions
    GLCALL(glEnableVertexAttribArray(0));
    GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
    // vertex normals
    GLCALL(glEnableVertexAttribArray(1));
    GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));
    // vertex texture coords
    GLCALL(glEnableVertexAttribArray(2));
    GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinate)));
//    // vertex tangent
//    glEnableVertexAttribArray(3);
//    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
//    // vertex bitangent
//    glEnableVertexAttribArray(4);
//    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    GLCALL(glBindVertexArray(0));

    return vao;
}


std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, const std::string& type_name)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = load_texture(std::string("../resources/models/") + str.C_Str());
        texture.type = type_name;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}


Mesh process_mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        if (mesh->mNormals)
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        else
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);


        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_coordinate = vec;
        }
        else
            vertex.texture_coordinate = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }


    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }


    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = load_material_textures(material,
                                                                  aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = load_material_textures(material,
                                                                   aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    GLuint handle = create_mesh(vertices, indices);

    return {handle, vertices, indices, textures};
}


void process_node(aiNode* node, const aiScene* scene, std::vector<Mesh>& stack)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        unsigned int index = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[index];
        stack.push_back(process_mesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene, stack);
    }
}


GLFWwindow* create_window()
{
    stbi_set_flip_vertically_on_load(true);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    return window;
}


GLuint create_cube()
{
    // MESH
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
//    unsigned int indices[] = {  // note that we start from 0!
//            0, 1, 3,   // first triangle
//            1, 2, 3    // second triangle
//    };

    GLuint vao, vbo, ebo;
    GLCALL(glGenVertexArrays(1, &vao));
    GLCALL(glGenBuffers(1, &vbo));
//    GLCALL(glGenBuffers(1, &ebo));

    GLCALL(glBindVertexArray(vao));

    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

//    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
//    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_POSITION));
    GLCALL(glVertexAttribPointer(SHADER_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0));
    GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_NORMAL));
    GLCALL(glVertexAttribPointer(SHADER_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
    GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_TEXTURE_COORDINATE));
    GLCALL(glVertexAttribPointer(SHADER_LAYOUT_TEXTURE_COORDINATE, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));

    GLCALL(glBindVertexArray(0));

    return vao;
}


GLuint create_quad()
{
    float vertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

            -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    };

    GLuint vao, vbo;
    GLCALL(glGenVertexArrays(1, &vao));
    GLCALL(glGenBuffers(1, &vbo));

    GLCALL(glBindVertexArray(vao));

    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));


    GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_POSITION));
    GLCALL(glVertexAttribPointer(SHADER_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0));
    GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_TEXTURE_COORDINATE));
    GLCALL(glVertexAttribPointer(SHADER_LAYOUT_TEXTURE_COORDINATE, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(float))));

    GLCALL(glBindVertexArray(0));

    return vao;
}


GLuint create_skybox_cube()
{
    GLfloat vertices[] = {
            // positions
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };

    GLuint vao, vbo, ebo;
    GLCALL(glGenVertexArrays(1, &vao));
    GLCALL(glGenBuffers(1, &vbo));

    GLCALL(glBindVertexArray(vao));

    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_POSITION));
    GLCALL(glVertexAttribPointer(SHADER_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0));

    GLCALL(glBindVertexArray(0));

    return vao;
}



int main()
{
    GLFWwindow* window = create_window();


    // Transformation
    glm::mat4 model;
    glm::mat4 view ;
    glm::mat4 projection;
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 0.0f));
    view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);


    // Models
    GLuint quad_vao   = create_quad();
    GLuint cube_vao   = create_cube();
    GLuint skybox_vao = create_skybox_cube();

    // Texturing
    GLuint vegetation_texture = load_texture("../resources/images/grass.png", true);

    std::vector<std::string> cube_textures = {
            "../resources/skybox/ruins/right.tga",
            "../resources/skybox/ruins/left.tga",
            "../resources/skybox/ruins/top.tga",
            "../resources/skybox/ruins/bottom.tga",
            "../resources/skybox/ruins/back.tga",
            "../resources/skybox/ruins/front.tga"
    };
    GLuint cube_map_texture = CreateCubeMapTexture(cube_textures);



    // SHADERS
    GLuint instanced_program_id = create_program("../shaders/instancedv.glsl", "../shaders/instancedf.glsl");
    GLuint skybox_program_id = create_program("../shaders/skybox.vertex.glsl", "../shaders/skybox.fragment.glsl");
    GLuint basic_program_id  = create_program("../shaders/basicquadv.glsl", "../shaders/basicquadf.glsl");
    GLuint trans_program_id  = create_program("../shaders/flatv.glsl", "../shaders/flatf.glsl");

    ShaderProgram2 instanced_program;
    instanced_program.id = instanced_program_id;
    CacheUniforms(instanced_program, {"view", "projection"});

    ShaderProgram2 skybox_program;
    skybox_program.id = skybox_program_id;
    CacheUniforms(skybox_program, {"view", "projection", "skybox"});

    ShaderProgram2 basic_program;
    basic_program.id = basic_program_id;
    CacheUniforms(basic_program, {"view", "projection", "model"});

    ShaderProgram2 trans_program;
    trans_program.id = trans_program_id;
    CacheUniforms(trans_program, {"view", "projection", "model", "image"});
    GLCALL(glUniformMatrix4fv(trans_program.location["model"], 1, GL_FALSE, glm::value_ptr(model)));
    GLCALL(glUniformMatrix4fv(trans_program.location["view"],  1, GL_FALSE, glm::value_ptr(view)));
    GLCALL(glUniformMatrix4fv(trans_program.location["projection"],  1, GL_FALSE, glm::value_ptr(projection)));
    GLCALL(glUniform1i(trans_program.location["image"], 0));



    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    unsigned int amount = 100;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed
    float radius = 25.0;
    float offset = 10.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model;
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
//        float scale = (rand() % 20) / 100.0f + 0.05;
//        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
    // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
        unsigned int VAO = quad_vao;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);

    GLCALL(glEnable(GL_DEPTH_TEST));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        delta_time = currentFrame - last_time;
        last_time = currentFrame;

        // input
        // -----
        process_input(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

        GLCALL(glUseProgram(instanced_program.id));
        GLCALL(glUniformMatrix4fv(instanced_program.location["view"], 1, GL_FALSE, glm::value_ptr(view)));
        GLCALL(glUniformMatrix4fv(instanced_program.location["projection"],  1, GL_FALSE, glm::value_ptr(projection)));
        GLCALL(glActiveTexture(GL_TEXTURE0));
        GLCALL(glBindTexture(GL_TEXTURE_2D, vegetation_texture));

        {
            glBindVertexArray(VAO);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
            glBindVertexArray(0);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}