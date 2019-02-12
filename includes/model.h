//
// Created by Ted Klein Bergman on 2019-01-10.
//
#pragma once

#include <bitset>

#include <glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "gl_debug.h"
#include "shader.h"
#include "texture.h"


struct Vertex
{
    glm::vec3 position;
    glm::vec2 texture_coordinate;
    glm::vec3 normal;
};



struct Model
{
    // STATIC METHODS

    static Model cube()
    {
        // MESH
        GLfloat vertices[] = {
                // positions           // texture coords        // normals
                -0.5f, -0.5f, -0.5f,     0.0f,  0.0f,         0.0f,  0.0f, -1.0f,
                0.5f, -0.5f, -0.5f,     1.0f,  0.0f,         0.0f,  0.0f, -1.0f,
                0.5f,  0.5f, -0.5f,     1.0f,  1.0f,         0.0f,  0.0f, -1.0f,
                0.5f,  0.5f, -0.5f,     1.0f,  1.0f,         0.0f,  0.0f, -1.0f,
                -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,         0.0f,  0.0f, -1.0f,
                -0.5f, -0.5f, -0.5f,     0.0f,  0.0f,         0.0f,  0.0f, -1.0f,

                -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,         0.0f,  0.0f,  1.0f,
                0.5f, -0.5f,  0.5f,     1.0f,  0.0f,         0.0f,  0.0f,  1.0f,
                0.5f,  0.5f,  0.5f,     1.0f,  1.0f,         0.0f,  0.0f,  1.0f,
                0.5f,  0.5f,  0.5f,     1.0f,  1.0f,         0.0f,  0.0f,  1.0f,
                -0.5f,  0.5f,  0.5f,     0.0f,  1.0f,         0.0f,  0.0f,  1.0f,
                -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,         0.0f,  0.0f,  1.0f,

                -0.5f,  0.5f,  0.5f,     1.0f,  0.0f,        -1.0f,  0.0f,  0.0f,
                -0.5f,  0.5f, -0.5f,     1.0f,  1.0f,        -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f,     0.0f,  1.0f,        -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f,     0.0f,  1.0f,        -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,        -1.0f,  0.0f,  0.0f,
                -0.5f,  0.5f,  0.5f,     1.0f,  0.0f,        -1.0f,  0.0f,  0.0f,

                0.5f,  0.5f,  0.5f,     1.0f,  0.0f,         1.0f,  0.0f,  0.0f,
                0.5f,  0.5f, -0.5f,     1.0f,  1.0f,         1.0f,  0.0f,  0.0f,
                0.5f, -0.5f, -0.5f,     0.0f,  1.0f,         1.0f,  0.0f,  0.0f,
                0.5f, -0.5f, -0.5f,     0.0f,  1.0f,         1.0f,  0.0f,  0.0f,
                0.5f, -0.5f,  0.5f,     0.0f,  0.0f,         1.0f,  0.0f,  0.0f,
                0.5f,  0.5f,  0.5f,     1.0f,  0.0f,         1.0f,  0.0f,  0.0f,

                -0.5f, -0.5f, -0.5f,     0.0f,  1.0f,         0.0f, -1.0f,  0.0f,
                0.5f, -0.5f, -0.5f,     1.0f,  1.0f,         0.0f, -1.0f,  0.0f,
                0.5f, -0.5f,  0.5f,     1.0f,  0.0f,         0.0f, -1.0f,  0.0f,
                0.5f, -0.5f,  0.5f,     1.0f,  0.0f,         0.0f, -1.0f,  0.0f,
                -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,         0.0f, -1.0f,  0.0f,
                -0.5f, -0.5f, -0.5f,     0.0f,  1.0f,         0.0f, -1.0f,  0.0f,

                -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,         0.0f,  1.0f,  0.0f,
                0.5f,  0.5f, -0.5f,     1.0f,  1.0f,         0.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,     1.0f,  0.0f,         0.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,     1.0f,  0.0f,         0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f,  0.5f,     0.0f,  0.0f,         0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,          0.0f,  1.0f,  0.0f
        };

        GLuint vao, vbo;
        GLCALL(glGenVertexArrays(1, &vao));
        GLCALL(glGenBuffers(1, &vbo));

        GLCALL(glBindVertexArray(vao));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

        unsigned size = 8 * sizeof(GLfloat);
        long offset   = 0;

        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0));
        GLCALL(glEnableVertexAttribArray(1));
        GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
        GLCALL(glEnableVertexAttribArray(2));
        GLCALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))));

        GLCALL(glBindVertexArray(0));

        std::bitset<32> attributes;
        attributes.set(0);
        attributes.set(1);
        attributes.set(2);

        return {vao, attributes, 36};
    }

    static Model quad()
    {
        float vertices[] = {
                // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
                -1.0f, -1.0f,  0.0f,     0.0f,  0.0f,       0.0f, 0.0f, -1.0f,
                1.0f,  1.0f,  0.0f,     1.0f,  1.0f,        0.0f, 0.0f, -1.0f,
                -1.0f,  1.0f,  0.0f,     0.0f,  1.0f,       0.0f, 0.0f, -1.0f,

                -1.0f, -1.0f,  0.0f,     0.0f,  0.0f,       0.0f, 0.0f, -1.0f,
                1.0f, -1.0f,  0.0f,     1.0f,  0.0f,        0.0f, 0.0f, -1.0f,
                1.0f,  1.0f,  0.0f,     1.0f,  1.0f,        0.0f, 0.0f, -1.0f,
        };

        GLuint vao, vbo;
        GLCALL(glGenVertexArrays(1, &vao));
        GLCALL(glGenBuffers(1, &vbo));

        GLCALL(glBindVertexArray(vao));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));


        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0));
        GLCALL(glEnableVertexAttribArray(1));
        GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(float))));
        GLCALL(glEnableVertexAttribArray(2));
        GLCALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(float))));

        GLCALL(glBindVertexArray(0));

        std::bitset<32> attributes;
        attributes.set(0);
        attributes.set(1);
        attributes.set(2);

        return {vao, attributes, 6};
    }


    // STATIC VARIABLES
    static GLuint bound_model;  // Initialize to 0 below.


    // INSTANCE VARIABLES AND FUNCTIONS

    // VAO handle.
    GLuint id;

    // Attribute bitset to check whether a model is compatible with a shader.
    std::bitset<32> attributes;
    GLuint count;
    GLenum draw_mode = GL_TRIANGLES;

    void enable()
    {
        Model::bound_model = id;
        GLCALL(glBindVertexArray(id));
    }

    void draw()
    {
        if (Model::bound_model != id)
            throw std::runtime_error("Model " + std::to_string(id) + " isn't currently bound. Model " + std::to_string(Model::bound_model) + " is bound!");
        GLCALL(glDrawArrays(draw_mode, 0, count));
    }

};
GLuint Model::bound_model = 0;

struct IndexedModel
{
    static IndexedModel create(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
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
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW));

        // Set the vertex attribute pointers
        // Positions
        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));

        // Texture coordinates
        GLCALL(glEnableVertexAttribArray(1));
        GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinate)));

        // Normals
        GLCALL(glEnableVertexAttribArray(2));
        GLCALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));

        GLCALL(glBindVertexArray(0));

        std::bitset<32> attributes;
        attributes.set(SHADER_LAYOUT_POSITION);
        attributes.set(SHADER_LAYOUT_TEXTURE_COORDINATE);
        attributes.set(SHADER_LAYOUT_NORMAL);

        return {vao, attributes, static_cast<GLuint>(indices.size())};
    }


    // STATIC VARIABLES
    static GLuint bound_model;  // Initialize to 0 below.


    // VAO handle.
    GLuint id;

    // Attribute bitset to check whether a model is compatible with a shader.
    std::bitset<32> attributes;

    GLuint count;

    GLenum draw_mode  = GL_TRIANGLES;
    GLenum index_type = GL_UNSIGNED_INT;

    void enable()
    {
        IndexedModel::bound_model = id;
        GLCALL(glBindVertexArray(id));
    }

    void draw()
    {
        if (IndexedModel::bound_model != id)
            throw std::runtime_error("IndexedModel " + std::to_string(id) + " isn't currently bound. Model " + std::to_string(Model::bound_model) + " is bound!");
        GLCALL(glDrawElements(draw_mode, count, index_type, nullptr));
    }
};
GLuint IndexedModel::bound_model = 0;



struct TexturedModel
{
    IndexedModel model;
    std::vector<Texture> textures;

    void draw()
    {
        model.enable();
        model.draw();
    }
};



//  glDrawArrays(GL_TRIANGLES, 0, 36);
//  glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
//
// GLuint create_cube()
// {
//     // MESH
//     float vertices[] = {
//             // positions          // normals           // texture coords
//             -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
//             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
//             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
//             -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
//             -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//
//             -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
//             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
//             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
//             -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
//             -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//
//             -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//             -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
//             -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//             -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//             -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
//             -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//
//             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
//             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
//             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//
//             -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
//             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
//             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
//             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
//             -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
//             -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
//
//             -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
//             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
//             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
//             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
//             -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
//             -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
//     };
// //    unsigned int indices[] = {  // note that we start from 0!
// //            0, 1, 3,   // first triangle
// //            1, 2, 3    // second triangle
// //    };
//
//     GLuint vao, vbo, ebo;
//     GLCALL(glGenVertexArrays(1, &vao));
//     GLCALL(glGenBuffers(1, &vbo));
// //    GLCALL(glGenBuffers(1, &ebo));
//
//     GLCALL(glBindVertexArray(vao));
//
//     GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
//     GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
//
// //    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
// //    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
//
//     GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_POSITION));
//     GLCALL(glVertexAttribPointer(SHADER_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0));
//     GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_NORMAL));
//     GLCALL(glVertexAttribPointer(SHADER_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
//     GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_TEXTURE_COORDINATE));
//     GLCALL(glVertexAttribPointer(SHADER_LAYOUT_TEXTURE_COORDINATE, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
//
//     GLCALL(glBindVertexArray(0));
//
//     return vao;
// }
//
//
// GLuint create_quad()
// {
//     float vertices[] = {
//             // positions                // texture Coords (swapped y coordinates because texture is flipped upside down)
//             -1.0f, -1.0f,  0.0f,         0.0f,  0.0f,
//             1.0f,  1.0f,  0.0f,         1.0f,  1.0f,
//             -1.0f,  1.0f,  0.0f,         0.0f,  1.0f,
//
//             -1.0f, -1.0f,  0.0f,         0.0f,  0.0f,
//             1.0f, -1.0f,  0.0f,         1.0f,  0.0f,
//             1.0f,  1.0f,  0.0f,         1.0f,  1.0f,
//     };
//
//     GLuint vao, vbo;
//     GLCALL(glGenVertexArrays(1, &vao));
//     GLCALL(glGenBuffers(1, &vbo));
//
//     GLCALL(glBindVertexArray(vao));
//
//     GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
//     GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
//
//
//     GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_POSITION));
//     GLCALL(glVertexAttribPointer(SHADER_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0));
//     GLCALL(glEnableVertexAttribArray(SHADER_LAYOUT_TEXTURE_COORDINATE));
//     GLCALL(glVertexAttribPointer(SHADER_LAYOUT_TEXTURE_COORDINATE, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(float))));
//
//     GLCALL(glBindVertexArray(0));
//
//     return vao;
// }
//
// Model create_skybox_cube()
// {
//     GLfloat vertices[] = {
//             // positions
//             -1.0f, 1.0f, -1.0f,
//             -1.0f, -1.0f, -1.0f,
//             1.0f, -1.0f, -1.0f,
//             1.0f, -1.0f, -1.0f,
//             1.0f, 1.0f, -1.0f,
//             -1.0f, 1.0f, -1.0f,
//
//             -1.0f, -1.0f, 1.0f,
//             -1.0f, -1.0f, -1.0f,
//             -1.0f, 1.0f, -1.0f,
//             -1.0f, 1.0f, -1.0f,
//             -1.0f, 1.0f, 1.0f,
//             -1.0f, -1.0f, 1.0f,
//
//             1.0f, -1.0f, -1.0f,
//             1.0f, -1.0f, 1.0f,
//             1.0f, 1.0f, 1.0f,
//             1.0f, 1.0f, 1.0f,
//             1.0f, 1.0f, -1.0f,
//             1.0f, -1.0f, -1.0f,
//
//             -1.0f, -1.0f, 1.0f,
//             -1.0f, 1.0f, 1.0f,
//             1.0f, 1.0f, 1.0f,
//             1.0f, 1.0f, 1.0f,
//             1.0f, -1.0f, 1.0f,
//             -1.0f, -1.0f, 1.0f,
//
//             -1.0f, 1.0f, -1.0f,
//             1.0f, 1.0f, -1.0f,
//             1.0f, 1.0f, 1.0f,
//             1.0f, 1.0f, 1.0f,
//             -1.0f, 1.0f, 1.0f,
//             -1.0f, 1.0f, -1.0f,
//
//             -1.0f, -1.0f, -1.0f,
//             -1.0f, -1.0f, 1.0f,
//             1.0f, -1.0f, -1.0f,
//             1.0f, -1.0f, -1.0f,
//             -1.0f, -1.0f, 1.0f,
//             1.0f, -1.0f, 1.0f
//     };
//
//     GLuint vao, vbo;
//     GLCALL(glGenVertexArrays(1, &vao));
//     GLCALL(glGenBuffers(1, &vbo));
//
//     GLCALL(glBindVertexArray(vao));
//
//     GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
//     GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
//
//     GLCALL(glEnableVertexAttribArray(0));
//     GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0));
//
//     GLCALL(glBindVertexArray(0));
//
//     std::bitset<32> mask;
//     mask.set(0);
//
//     return {vao, mask, 36};
// }
//








std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType ai_type, Texture::Type type)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(ai_type); i++)
    {
        aiString str;
        mat->GetTexture(ai_type, i, &str);

        std::string path = std::string("../resources/models/") + str.C_Str();
        Texture texture  = Texture::create(ImageData::create(path), type);

        textures.push_back(texture);
    }
    return textures;
}

std::tuple<std::vector<Vertex>, std::vector<GLuint>, std::vector<Texture>> process_mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;


        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_coordinate = vec;
        }
        else
        {
            vertex.texture_coordinate = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->mNormals)
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        else
        {
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        if (mesh->mBitangents)
        {
            throw std::runtime_error("Don't handle Bitangets yet!");
        }

        if (mesh->mTangents)
        {
            throw std::runtime_error("Don't handle Tangets yet!");
        }

        vertices.push_back(vertex);
    }


    // process indices
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }


    // process material
    std::vector<Texture> textures;
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuse_textures  = load_material_textures(material, aiTextureType_DIFFUSE,  Texture::Type::DIFFUSE);
        std::vector<Texture> specular_textures = load_material_textures(material, aiTextureType_SPECULAR, Texture::Type::SPECULAR);
        std::vector<Texture> normal_textures   = load_material_textures(material, aiTextureType_NORMALS,  Texture::Type::NORMAL);
        std::vector<Texture> emission_textures = load_material_textures(material, aiTextureType_EMISSIVE, Texture::Type::EMISSION);


        textures.reserve(textures.size() + diffuse_textures.size() + specular_textures.size() + normal_textures.size() + emission_textures.size());
        textures.insert(textures.end(), diffuse_textures.begin(),  diffuse_textures.end());
        textures.insert(textures.end(), specular_textures.begin(), specular_textures.end());
        textures.insert(textures.end(), normal_textures.begin(),   normal_textures.end());
        textures.insert(textures.end(), emission_textures.begin(), emission_textures.end());
    }


    return {vertices, indices, textures};
}


std::vector<TexturedModel> process_node(aiNode* node, const aiScene* scene)
{
    std::vector<TexturedModel> models;

    // Process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        unsigned int index = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[index];

        std::vector<Vertex>  vertices;
        std::vector<GLuint>  indices;
        std::vector<Texture> textures;

        std::tie(vertices, indices, textures) = process_mesh(mesh, scene);

        IndexedModel new_model = IndexedModel::create(vertices, indices);
        models.push_back({new_model, textures});
    }

    // Do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {

        auto new_models = process_node(node->mChildren[i], scene);

        models.reserve(models.size() + new_models.size());
        models.insert(models.end(), new_models.begin(), new_models.end());
    }

    return models;
}

std::vector<TexturedModel> load_obj_file(const std::string& path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw std::runtime_error("ERROR::ASSIMP::" + std::string(import.GetErrorString()));

    return process_node(scene->mRootNode, scene);
}

//
// std::tuple<std::vector<Vertex>, std::vector<GLuint>, std::vector<Texture>> process_node(aiNode* node, const aiScene* scene)
// {
//     std::vector<Vertex>  vertex_data;
//     std::vector<GLuint>  index_data;
//     std::vector<Texture> texture_data;
//
//     // Process all the node's meshes (if any)
//     for (unsigned int i = 0; i < node->mNumMeshes; ++i)
//     {
//         unsigned int index = node->mMeshes[i];
//         aiMesh* mesh = scene->mMeshes[index];
//
//         std::vector<Vertex>  vertices;
//         std::vector<GLuint>  indices;
//         std::vector<Texture> textures;
//
//         std::tie(vertices, indices, textures) = process_mesh(mesh, scene);
//
//         vertex_data.insert(vertex_data.end(),   vertices.begin(), vertices.end());
//         index_data.insert(index_data.end(),     indices.begin(),  indices.end());
//         texture_data.insert(texture_data.end(), textures.begin(), textures.end());
//     }
//
//     // Do the same for each of its children
//     for (unsigned int i = 0; i < node->mNumChildren; i++)
//     {
//         std::vector<Vertex>  vertices;
//         std::vector<GLuint>  indices;
//         std::vector<Texture> textures;
//
//         std::tie(vertices, indices, textures) = process_node(node->mChildren[i], scene);
//
//         vertex_data.insert(vertex_data.end(),   vertices.begin(), vertices.end());
//         index_data.insert(index_data.end(),     indices.begin(),  indices.end());
//         texture_data.insert(texture_data.end(), textures.begin(), textures.end());
//     }
//
//     return {vertex_data, index_data, texture_data};
// }
//
//
//
// TexturedModel load_obj_file(const std::string& path)
// {
//     Assimp::Importer import;
//     const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);
//
//     if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//         throw std::runtime_error("ERROR::ASSIMP::" + std::string(import.GetErrorString()));
//
//     std::vector<Vertex>  vertices;
//     std::vector<GLuint>  indices;
//     std::vector<Texture> textures;
//
//     std::tie(vertices, indices, textures) = process_node(scene->mRootNode, scene);
//
//     auto model = IndexedModel::create(vertices, indices);
//     return {model, textures};
// }


