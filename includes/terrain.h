//
// Created by Ted Klein Bergman on 2019-01-11.
//

#pragma once

#include <vector>

#include "model.h"
#include "texture.h"
#include "mathematics.h"


class Terrain
{
public:

    static Terrain create(const ImageData& height_map, float max_height = 1.0f)
    {
        unsigned count = height_map.width * height_map.height;

        // std::vector<Vertex> vertices(count);
        std::vector<GLuint> indices(6 * (height_map.width - 1) * (height_map.height - 1));

        Array2D<Vertex> vertices (height_map.width, height_map.height);
        Array2D<float>  heights  (height_map.width, height_map.height);

        int vertex_index = 0;
        for (unsigned z = 0; z < height_map.height; ++z)
        {
            for (unsigned x = 0; x < height_map.width; ++x)
            {
                float last_z = static_cast<float>(height_map.height - 1);
                float last_x = static_cast<float>(height_map.width  - 1);

                Vertex& vertex = vertices(x, z);

                unsigned row    = static_cast<unsigned>(lerp(0, static_cast<float>(height_map.height), static_cast<float>(z)/static_cast<float>(samples)));
                unsigned column = static_cast<unsigned>(lerp(0, static_cast<float>(height_map.width),  static_cast<float>(x)/static_cast<float>(samples)));


                glm::vec3& position = vertex.position;
                position.x = x / last_x;
                position.y = lerp(0, max_height, height_map.at(row, column) / static_cast<float>(UCHAR_MAX));
                position.z = z / last_z;

                glm::vec2& texture_coordinate = vertex.texture_coordinate;
                texture_coordinate.x = static_cast<GLfloat>(x) / last_x;
                texture_coordinate.y = static_cast<GLfloat>(z) / last_z;

                glm::vec3& normal = vertex.normal;
                normal.x = 0;
                normal.y = 1;
                normal.z = 0;

                heights(x, z) = position.y;

                ++vertex_index;
            }
        }


        int index_pointer = 0;
        for (unsigned z = 0; z < height_map.height - 1; ++z)
        {
            for (unsigned x = 0; x < height_map.width - 1; ++x)
            {
                GLuint top_left     = (z * height_map.width) + x;
                GLuint top_right    = top_left + 1;
                GLuint bottom_left  = (z + 1) *  height_map.width + x;
                GLuint bottom_right = bottom_left + 1;

                indices[index_pointer++] = top_left;
                indices[index_pointer++] = bottom_left;
                indices[index_pointer++] = top_right;
                indices[index_pointer++] = top_right;
                indices[index_pointer++] = bottom_left;
                indices[index_pointer++] = bottom_right;
            }
        }

        IndexedModel model = IndexedModel::create(vertices.data(), indices);
        return {model, heights};
    }


    IndexedModel model;
    Array2D<float> heights;
};



IndexedMesh generate_terrain(const ImageData& height_map, float max_height = 1.0f)
{
    unsigned count = height_map.width * height_map.height;

    std::vector<Vertex> vertices(count);
    std::vector<GLuint> indices(6 * (height_map.width - 1) * (height_map.height - 1));

    Array2D<float> heights(height_map.width, height_map.height);

    int vertex_index = 0;
    for (unsigned z = 0; z < height_map.height; ++z)
    {
        for (unsigned x = 0; x < height_map.width; ++x)
        {
            float last_z = static_cast<float>(height_map.height - 1);
            float last_x = static_cast<float>(height_map.width  - 1);

            Vertex& vertex = vertices[vertex_index];

            unsigned row    = static_cast<unsigned>(lerp(0, static_cast<float>(height_map.height), static_cast<float>(z)/static_cast<float>(samples)));
            unsigned column = static_cast<unsigned>(lerp(0, static_cast<float>(height_map.width),  static_cast<float>(x)/static_cast<float>(samples)));


            glm::vec3& position = vertex.position;
            position.x = x / last_x;
            position.y = lerp(0, max_height, height_map.at(row, column) / static_cast<float>(UCHAR_MAX));
            position.z = z / last_z;

            glm::vec2& texture_coordinate = vertex.texture_coordinate;
            texture_coordinate.x = static_cast<GLfloat>(x) / last_x;
            texture_coordinate.y = static_cast<GLfloat>(z) / last_z;

            glm::vec3& normal = vertex.normal;
            normal.x = 0;
            normal.y = 1;
            normal.z = 0;

            heights(x, z) = position.y;

            ++vertex_index;
        }
    }


    int index_pointer = 0;
    for (unsigned z = 0; z < height_map.height - 1; ++z)
    {
        for (unsigned x = 0; x < height_map.width - 1; ++x)
        {
            GLuint top_left     = (z * height_map.width) + x;
            GLuint top_right    = top_left + 1;
            GLuint bottom_left  = (z + 1) *  height_map.width + x;
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

