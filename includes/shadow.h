//
// Created by Ted Klein Bergman on 2019-01-13.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_debug.h"
#include "shader.h"
#include "utilities.h"
#include "model.h"


class Shadow
{
    static Shadow create(unsigned width, unsigned height)
    {
        // SHADOW MAP
        GLuint shadow_map_fbo;
        GLCALL(glGenFramebuffers(1, &shadow_map_fbo));

        GLuint shadow_map;
        GLCALL(glGenTextures(1, &shadow_map));
        GLCALL(glBindTexture(GL_TEXTURE_2D, shadow_map));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo));
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map, 0));
        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glReadBuffer(GL_NONE));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        return {shadow_map, shadow_map_fbo, width, height};
    }


    static ShaderProgram shader;

    GLuint map, fbo;
    unsigned width, height;


    int generate_shadow(Model model, const std::vector<glm::vec3>& positions, glm::vec3 light_position, glm::vec3 light_direction)
    {
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);
        glm::mat4 light_view = glm::lookAt(light_position, light_position + light_direction, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 light_projection_view = light_projection * light_view;

        // Shadow
        GLCALL(glCullFace(GL_FRONT));  // Better looking shadows.
        GLCALL(glViewport(0, 0, width, height));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
        GLCALL(glClear(GL_DEPTH_BUFFER_BIT));


        shader.enable();
        shader.load("light_projection_view", light_projection_view);
        for (auto& position : positions)
        {
            glm::mat4 model_matrix = {};
            model_matrix = glm::translate(model_matrix, position);
            shader.load("model", model_matrix);
            model.draw();
        }


        GLCALL(glCullFace(GL_BACK));
        // GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));           <-- How do we fix?
        GLCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
        // GLCALL(glViewport(0, 0, window.width, window.height));    <-- How do we fix?
    }


};
ShaderProgram Shadow::shader = ShaderProgram::create({
    ShaderProgram::vertex(read("../shaders/shadowv.glsl")), ShaderProgram::fragment(read("../shaders/shadowf.glsl"))
});