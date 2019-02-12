//
// Created by Ted Klein Bergman on 10/31/18.
//
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct OrientationInfo
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f,  0.0f);

    // Direction vectors
    glm::vec3 front    = glm::vec3(0.0f, 0.0f, -1.0f);  // Change for pitch and yaw.
    glm::vec3 up       = glm::vec3(0.0f, 1.0f,  0.0f);  // Change for rolls.
};


struct PerspectiveInfo
{
    float aspect_ratio = 1.0f;
    float fov  = 45.0f;
    float near = 0.1f;
    float far  = 100.0f;
};


struct Camera
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f,  0.0f);

    // Direction vectors
    glm::vec3 front    = glm::vec3(0.0f, 0.0f, -1.0f);  // Change for pitch and yaw.
    glm::vec3 up       = glm::vec3(0.0f, 1.0f,  0.0f);  // Change for rolls.

    float aspect_ratio = 1.0f;
    float fov  = 45.0f;
    float near = 0.1f;
    float far  = 100.0f;



    glm::mat4 view()
    {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 perspective()
    {
        return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
    }
};

