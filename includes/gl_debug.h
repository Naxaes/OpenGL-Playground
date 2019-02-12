//
// Created by Ted Klein Bergman on 1/19/18.
//

#ifndef GRAPHICPROJECT_GL_DEBUG_H
#define GRAPHICPROJECT_GL_DEBUG_H

#include "glad/glad.h"
#include <iostream>
#include <vector>

#define DEBUG true

#if DEBUG
#define GLCALL(x) clear_errors(); x; print_errors(#x, __FILE__, __LINE__)
#else
#define GLCALL(x) x
#endif

#define GET_VARIABLE_NAME(x) #x
#define check_shader_status(a, b) _check_shader_status(a, b, #b)
#define check_program_status(a, b) _check_program_status(a, b, #b)


const char* error_to_string(GLuint error)
{
    const char* error_message;

    switch (error)
    {
        case GL_INVALID_ENUM:
            error_message = GET_VARIABLE_NAME(GL_INVALID_ENUM);
            break;
        case GL_INVALID_VALUE:
            error_message = GET_VARIABLE_NAME(GL_INVALID_VALUE);
            break;
        case GL_INVALID_OPERATION:
            error_message = GET_VARIABLE_NAME(GL_INVALID_OPERATION);
            break;
        case GL_STACK_OVERFLOW:
            error_message = GET_VARIABLE_NAME(GL_STACK_OVERFLOW);
            break;
        case GL_STACK_UNDERFLOW:
            error_message = GET_VARIABLE_NAME(GL_STACK_UNDERFLOW);
            break;
        case GL_OUT_OF_MEMORY:
            error_message = GET_VARIABLE_NAME(GL_OUT_OF_MEMORY);
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error_message = GET_VARIABLE_NAME(GL_INVALID_FRAMEBUFFER_OPERATION);
            break;
        default:
            error_message = "UNKNOWN_ERROR";
    }

    return error_message;
}


void clear_errors()
{
    while (glGetError() != GL_NO_ERROR);
}


void print_errors(const char* function_name, const char* file_name, int line)
{
    GLuint error;

    while ((error = glGetError()) != GL_NO_ERROR)
    {
        fprintf(
                stderr,
                "[OpenGL Error] (%s %i):\n\tFunction: %s\n\tFile:     %s\n\tLine:     %i\n",
                error_to_string(error), error, function_name, file_name, line
        );
        std::exit(error);
    }
}


bool _check_shader_status(GLuint shaderID, GLuint status, const char* status_name)
{
    int success = 0;

    glGetShaderiv(shaderID, status, &success);

    if (!success)
    {
        int log_size;

        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &log_size);

        char log[log_size];

        glGetShaderInfoLog(shaderID, log_size, nullptr, log);

        std::string formatted_log;

        formatted_log.push_back('\t');
        for (const char character : log)
        {
            formatted_log.push_back(character);
            if (character == '\n') {
                formatted_log.push_back('\t');
            }
        }

        fprintf(
                stderr,
                "[Shader Error] (ID %i, %s):\n%s\n",
                shaderID, status_name, formatted_log.c_str()
        );

        return false;
    }

    return true;
}


bool _check_program_status(GLuint programID, GLuint status, const char* status_name)
{
    int success = 0;

    glGetProgramiv(programID, status, &success);

    if (!success) {
        int log_size;

        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &log_size);

        char log[log_size];

        glGetProgramInfoLog(programID, log_size, nullptr, log);

        std::string formatted_log;

        formatted_log.push_back('\t');
        for (const char character : log) {
            formatted_log.push_back(character);
            if (character == '\n') {
                formatted_log.push_back('\t');
            }
        }

        fprintf(
                stderr,
                "[Shader Program Error] (ID %i, %s):\n%s\n",
                programID, status_name, formatted_log.c_str()
        );

        return false;
    }

    return true;
}

#endif //GRAPHICPROJECT_GL_DEBUG_H
