//
// Created by Ted Klein Bergman on 10/31/18.
//

#pragma once

#include <string>
#include <unordered_map>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_debug.h"

// Attribute names and values
//     position : 0
//     texture_coordinate : 1
//     normal : 2
//     color : 3
//     position - 0
//     position - 0
//     position - 0
//     position - 0

constexpr GLuint SHADER_LAYOUT_POSITION = 0;
constexpr GLuint SHADER_LAYOUT_TEXTURE_COORDINATE = 1;
constexpr GLuint SHADER_LAYOUT_NORMAL = 2;
//
// std::unordered_map<std::string, unsigned> attribute_mapping {
//         {"position", 0},
//         {"texture_coordinate", 1},
//         {"normal", 2},
//         {"color",  3},
// };




struct Shader {
    // CLASS METHODS.
    static Shader vertex   (const std::string& source) { return create(source, GL_VERTEX_SHADER);   }
    static Shader fragment (const std::string& source) { return create(source, GL_FRAGMENT_SHADER); }
    static Shader geometry (const std::string& source) { return create(source, GL_GEOMETRY_SHADER); }

    static Shader create(std::string source, GLenum type)
    {
        const char* c_source = source.c_str();

        GLCALL(GLuint id = glCreateShader(type));
        GLCALL(glShaderSource(id, 1, &c_source, nullptr));
        GLCALL(glCompileShader(id));

        int success;
        GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            constexpr unsigned size = 512;
            char info[size];
            GLCALL(glGetShaderInfoLog(id, size, nullptr, info));
            throw std::runtime_error(std::string(info) + "\n\n" + source);
        }

        return {id, source, type};
    }

    const GLuint id;
    const std::string source;
    const GLenum type;

    // ~Shader() { GLCALL(glDeleteShader(id)); }
};


struct UniformInfo {
    std::string name;
    GLuint      location;
    GLint       size;
    GLenum      type;
};


struct UniformBlock {
    GLuint id;
    unsigned index;
};


class ShaderProgram {
private:
    static GLuint bound_shader;

public:

    // CLASS VARIABLES.
    static constexpr GLuint ATTRIBUTE_INDEX_POSITION = 0;
    static constexpr GLuint ATTRIBUTE_INDEX_TEXTURE_COORDINATE = 1;
    static constexpr GLuint ATTRIBUTE_INDEX_NORMAL   = 2;

    static std::unordered_map<std::string, UniformBlock> shared;

    static ShaderProgram create(std::vector<Shader> shaders)
    {
        // CREATE PROGRAM
        GLCALL(GLuint id = glCreateProgram());

        for (auto shader : shaders)
        {
            GLCALL(glAttachShader(id, shader.id));
        }

        GLCALL(glLinkProgram(id));
        GLCALL(glValidateProgram(id));

        // DEBUG PROGRAM
        int success;
        GLCALL(glGetProgramiv(id, GL_LINK_STATUS, &success));
        if (!success)
        {
            constexpr unsigned size = 512;
            char info[size];
            GLCALL(glGetProgramInfoLog(id, 512, nullptr, info));

            std::string error = std::string("\nError:\n") + info;
            for (auto shader : shaders)
                error += "\n" + shader.source;

            throw std::runtime_error(error);
        }


        // UNIFORM-NAME to UNIFORM-INFO mapping.
        GLint count;
        GLint max_attribute_name_length;
        GLCALL(glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count));
        GLCALL(glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_attribute_name_length));

        std::unordered_map<std::string, UniformInfo> uniform_info_mapping;
        for (GLuint i = 0; i < count; i++)
        {
            GLsizei length;
            GLint   size;
            GLenum  type;
            GLchar* c_name = new GLchar[max_attribute_name_length];

            GLCALL(glGetActiveUniform(id, i, max_attribute_name_length, &length, &size, &type, c_name));
            GLCALL(GLint location = glGetUniformLocation(id, c_name));

            std::string name(c_name);

            if (location >= 0)
                uniform_info_mapping[name] = UniformInfo{name, static_cast<GLuint>(location), size, type};
                // throw std::runtime_error("No active uniform with the name '" + name + "'.");

        }


        return {id, uniform_info_mapping};
    }

    static void create_shared_uniform_block(const std::string& uniform_block_name, unsigned size)
    {
        static unsigned index = 0;

        GLuint buffer;
        GLCALL(glGenBuffers(1, &buffer));
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, buffer));
        GLCALL(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));  // GL_DYNAMIC_DRAW
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));

        shared[uniform_block_name] = {buffer, index++};
    }

    template <typename T>
    static void load_shared_uniform_block(const std::string& uniform_block_name, T data, unsigned size, unsigned offset = 0)
    {
        auto uniform_block = shared[uniform_block_name];
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, uniform_block.id));
        GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
    }



    // INSTANCE VARIABLES.
    const GLuint id;
    const std::unordered_map<std::string, UniformInfo> uniform_info;


    // INSTANCE METHODS.
    ShaderProgram(
        GLuint id,
        std::unordered_map<std::string, UniformInfo> uniform_info
    ) : id(id),
        uniform_info(std::move(uniform_info))
    {}

    void enable()   const noexcept { GLCALL(glUseProgram(id)); ShaderProgram::bound_shader = id; };
    void disable()  const noexcept { GLCALL(glUseProgram(0));  ShaderProgram::bound_shader = 0; };
    bool is_bound() const noexcept { return ShaderProgram::bound_shader == id; };
    bool has_uniform(const std::string& uniform) const noexcept {
        return uniform_info.find(uniform) != uniform_info.end();
    };
    
    void assert_can_load(const std::string& uniform_name) const
    {
        if (!is_bound())
            throw std::runtime_error("Can't load uniform '" + uniform_name + "'. Shader " + std::to_string(id) + " is not bound (Shader " + std::to_string(ShaderProgram::bound_shader) + " is bound).");
        if (!has_uniform(uniform_name))
            throw std::runtime_error("Can't load uniform '" + uniform_name + "'. Shader " + std::to_string(id) + " does not have that uniform.");
    }

    void add_shared(const std::string& uniform_block_name) const
    {
        if (shared.find(uniform_block_name) == shared.end())
            throw std::runtime_error("Uniform block " + uniform_block_name + " has not been created.");

        auto uniform_block = shared[uniform_block_name];

        GLCALL(GLint block_index = glGetUniformBlockIndex(id, uniform_block_name.c_str()));
        if (block_index < 0)
            throw std::runtime_error("Uniform block " + uniform_block_name + " for shader " + std::to_string(id) + " doesn't exist.");

        GLCALL(glUniformBlockBinding(id, block_index, uniform_block.index));
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, uniform_block.id));
        GLCALL(glBindBufferBase(GL_UNIFORM_BUFFER, uniform_block.index, uniform_block.id));
    }

    void load_2D_textures(std::vector<GLuint> textures) const
    {
        for (unsigned int i = 0; i < textures.size(); ++i)
        {
            GLCALL(glActiveTexture(GL_TEXTURE0 + i));
            GLCALL(glBindTexture(GL_TEXTURE_2D, textures[i]));
        }
    }

    void load_3D_textures(std::vector<GLuint> textures) const
    {
        for (unsigned int i = 0; i < textures.size(); ++i)
        {
            GLCALL(glActiveTexture(GL_TEXTURE0 + i));
            GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i]));
        }
    }





    // Load overloads
    void load(const std::string& name, const glm::mat4& data) const
    {
        assert_can_load(name);
        GLCALL(glUniformMatrix4fv(uniform_info.at(name).location, 1, GL_FALSE, glm::value_ptr(data)));
    }

    void load(const std::string& name, const glm::vec3& data) const
    {
        assert_can_load(name);
        GLCALL(glUniform3fv(uniform_info.at(name).location, 1, &data[0]));
    }

    void load(const std::string& name, GLint data) const
    {
        assert_can_load(name);
        GLCALL(glUniform1i(uniform_info.at(name).location, data));
    }

    void load(const std::string& name, GLuint data) const
    {
        assert_can_load(name);
        GLCALL(glUniform1ui(uniform_info.at(name).location, data));
    }

    void load(const std::string& name, GLfloat data) const
    {
        assert_can_load(name);
        GLCALL(glUniform1f(uniform_info.at(name).location, data));
    }

    void load(const std::string& name, GLdouble data) const
    {
        assert_can_load(name);
        GLCALL(glUniform1d(uniform_info.at(name).location, data));
    }

    template <typename T>
    void load(const std::string& name, GLuint index, T data) const
    {
        load(name + "[" + std::to_string(index) + "]", data);
    }

    template <typename T>
    void load(const std::string& name, GLuint index, const std::string& attribute, T data) const
    {
        load(name + "[" + std::to_string(index) + "]." + attribute, data);
    }

    template <typename T>
    void load(const std::string& name, const std::string& attribute, T data) const
    {
        load(name + "." + attribute, data);
    }

};
GLuint ShaderProgram::bound_shader = 0;
std::unordered_map<std::string, UniformBlock> ShaderProgram::shared{};


