//
// Created by Ted Klein Bergman on 3/6/18.
//

#pragma

#include "obj_loader.h"

#include <cstdio>
#include <string>
#include <unordered_map>

#include "debug.h"


/*

 Parsing OBJ-file data.

 Example:

    v -1.000000 -1.000000 1.000000
    v -1.000000 1.000000 1.000000
    v -1.000000 -1.000000 -1.000000
    v -1.000000 1.000000 -1.000000
    v 1.000000 -1.000000 1.000000
    v 1.000000 1.000000 1.000000
    v 1.000000 -1.000000 -1.000000
    v 1.000000 1.000000 -1.000000
    vt 1.000000 0.000000
    vt 1.000000 1.000000
    vt 0.000000 1.000000
    vt 0.000000 0.000000
    vn -1.000000 0.000000 0.000000
    vn 0.000000 0.000000 -1.000000
    vn 1.000000 0.000000 0.000000
    vn 0.000000 0.000000 1.000000
    vn 0.000000 -1.000000 0.000000
    vn 0.000000 1.000000 0.000000
    f 4/1/1 3/2/1 1/3/1
    f 8/1/2 7/2/2 3/3/2
    f 6/1/3 5/2/3 7/3/3
    f 2/1/4 1/2/4 5/3/4
    f 3/1/5 7/2/5 5/3/5
    f 8/1/6 4/2/6 2/3/6
    f 2/4/1 4/1/1 1/3/1
    f 4/4/2 8/1/2 3/3/2
    f 8/4/3 6/1/3 7/3/3
    f 6/4/4 2/1/4 5/3/4
    f 1/4/5 3/1/5 5/3/5
    f 6/4/6 8/1/6 2/3/6


    i 0  1  2
    i 3  4  5
    i 6  7  8
    i 9  10 11
    i 12 13 14
    i 15 16 17
    i 18 0  2
    i 19 3  2

 1. Read all "v", "vt" and "vn" values into an array respectively.

 2. Create an array of checked vertices (initially 0).
 3. Create an array for all the indices (initially 0).
 4. Create an integer called index (initially 0).
 5. For each line with "f":
    1. Separate the line to three separate vertices.
    2. For each vertex:
        1. Check if it's in the checked vertices array.
        2. If it is not:
            1. Add the vertex in the checked array.
            2. Add the index in the indices array.
            3. Increment the index by 1.
        3. If it is:
            1. Add the index of the vertex in the indices array.





 */


char* get_subtring(const char* string, unsigned int start, unsigned int stop)
{
    unsigned int size = stop - start;
    char* substring = new char[size + 1];
    strncpy(substring, string + start, size);
    substring[size] = '\0';
    return substring;
}

std::vector<const char*> split_string(const char* string, const char delimiter)
{
    std::vector<const char*> result;

    unsigned int start = 0;
    unsigned int stop  = 1;  // In order to skip first character (in case it's a delimiter).

    while (string[stop] != '\0')
    {
        if (string[stop] == delimiter)
        {
            char* substring = get_subtring(string, start, stop);
            result.push_back(substring);

            stop += 1;
            start = stop;
        }

        stop += 1;
    }

    // Copy the remaining string as well.
    if (stop > start)
    {
        char* substring = get_subtring(string, start, stop-1);  // stop-1 to remove the new line character.
        result.push_back(substring);
    }

    return result;
}

// TODO(ted): bool flag warning... Should be two functions. Maybe have a normalize function.
OBJData load_obj_file(const char* path, bool normalize)
{
    FILE* file = fopen(path, "r");

    ASSERT(file, "Couldn't open file %s.", path);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texture_coordinates;
    std::vector<glm::vec3> normals;

    std::vector<unsigned int> positions_indices;
    std::vector<unsigned int> texture_coordinates_indices;
    std::vector<unsigned int> normals_indices;

    std::vector<unsigned int> indices;

    std::unordered_map<std::string, unsigned int> vertex_index;

    unsigned long index = 0;
    unsigned long count = 0;
    unsigned int  max_line_length = 128;
    float max_value = 1.0f;

    char line[max_line_length];
    while (fgets(line, max_line_length, file) != nullptr)
    {
        if (line[0] == 'v')
        {
            if (line[1] == 't')
            {
                glm::vec2 vertex;
                int matches = sscanf(line, "vt %f %f\n", &vertex.x, &vertex.y); // NOLINT

                ASSERT(matches == 2, "Couldn't parse file %s.", path);

                texture_coordinates.push_back(vertex);
            }
            else if (line[1] == 'n')
            {
                glm::vec3 vertex;
                int matches = sscanf(line, "vn %f %f %f\n", &vertex.x, &vertex.y, &vertex.z); // NOLINT

                ASSERT(matches == 3, "Couldn't parse file %s.", path);

                normals.push_back(vertex);
            }
            else
            {
                glm::vec3 vertex;
                int matches = sscanf(line, "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z); // NOLINT

                ASSERT(matches == 3, "Couldn't parse file %s.", path);

                if (vertex.x > max_value)
                    max_value = vertex.x;
                if (vertex.y > max_value)
                    max_value = vertex.y;
                if (vertex.z > max_value)
                    max_value = vertex.z;

                positions.push_back(vertex);
            }
        }
        else if (line[0] == 'f')
        {
            unsigned int position_index[3], texture_coordinate_index[3], normal_index[3];
            int matches = sscanf(
                    line, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
                    &position_index[0], &texture_coordinate_index[0], &normal_index[0],
                    &position_index[1], &texture_coordinate_index[1], &normal_index[1],
                    &position_index[2], &texture_coordinate_index[2], &normal_index[2]
            );

            ASSERT(matches == 9, "Couldn't parse file %s.", path);

            std::vector<const char *> vertices_data_string = split_string(line, OBJ_FACE_DATA_DELIMITER);
            for (unsigned int i = 1; i < 4; i++)
            {
                std::string vertex = vertices_data_string[i];
                bool vertex_exist = vertex_index.find(vertex) != vertex_index.end();

                if (vertex_exist)
                {
                    indices.push_back(vertex_index[vertex]);
                    continue;
                }
                else
                {
                    positions_indices.push_back(position_index[i-1]);
                    texture_coordinates_indices.push_back(texture_coordinate_index[i-1]);
                    normals_indices.push_back(normal_index[i-1]);
                    vertex_index[vertex] = static_cast<unsigned int>(index++);
                    indices.push_back(vertex_index[vertex]);
                }
            }
        }
    }


    std::vector<GLfloat> data;
    data.reserve(positions.size() + texture_coordinates.size() + normals.size());

    ASSERT(
            positions_indices.size() == texture_coordinates_indices.size() &&
            positions_indices.size() == normals_indices.size() &&
            normals_indices.size()   == texture_coordinates_indices.size(),
            "Something is wrong!"
    );
    count = positions_indices.size();

    if (!normalize)
        max_value = 1.0f;

    for (unsigned int i = 0; i < count; i++)
    {
        glm::vec3 position = positions[positions_indices[i] - 1];   // -1 since obj-file index starts at 1.
        data.push_back(position.x / max_value);
        data.push_back(position.y / max_value);
        data.push_back(position.z / max_value);

        glm::vec2 texture_coordinate = texture_coordinates[texture_coordinates_indices[i] - 1];  // -1 since obj-file index starts at 1.
        data.push_back(texture_coordinate.x);
        data.push_back(texture_coordinate.y);

        glm::vec3 normal = normals[normals_indices[i] - 1];   // -1 since obj-file index starts at 1.
        data.push_back(normal.x);
        data.push_back(normal.y);
        data.push_back(normal.z);
    }

    fclose(file);

    return {data, indices};
}








//int main () {
//    std::filebuf fb;
//    if (fb.open ("test.txt",std::ios::in))
//    {
//        std::istream is(&fb);
//        while (is)
//            std::cout << char(is.get());
//        fb.close();
//    }
//    return 0;
//}