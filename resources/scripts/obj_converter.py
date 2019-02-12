#!/usr/bin/env python3
import os
import sys

"""
---- FORMAT ----
// EVERYTHING ALWAYS LOWERCASE!

-- GENERAL EXAMPLE --
#options
    name   cube
    mode   TRIANGLE_STRIP
    
#attributes count=4
    position             count type normalized dimension
    normal               count type normalized dimension
    texture_coordinate   count type normalized dimension
    color                count type normalized dimension
    
#data count=197 interleaved=true
    [...]
    
#indices count=35 type=GL_UNSIGNED_INT            // OPTIONAL
    [...]
    
    
-- SPECIFIC EXAMPLE 1 --
#options
    name        cube
    mode        TRIANGLE
    indexed     true
    
#attributes count=4
    position             count=8  type=GL_FLOAT normalized=True dimension=3
    normal               count=36 type=GL_FLOAT normalized=True dimension=3
    texture_coordinate   count=6  type=GL_FLOAT normalized=True dimension=2
    color                count=8  type=GL_FLOAT normalized=True dimension=4
    
#data count=197 interleaved=true  // New line for each vertex
    -1.000000 1.000000 1.000000 0.000000 1.000000 -1.0000 -1.000000 1.000000 1.000000 0.000000 1.000000 -1.0000\n  
    -1.000000 1.000000 1.000000 0.000000 1.000000 -1.0000 -1.000000 1.000000 1.000000 0.000000 1.000000 -1.0000\n
    
#indices count=35 type=GL_UNSIGNED_INT
    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 0 18 1 3 19 4 6 20 7 9 21 10 12 22 13 15 23 16


-- SPECIFIC EXAMPLE 2 --
#options
    name        cube
    mode        TRIANGLE
    indexed     false
    
#attributes count=4
    position             count=8  type=GL_FLOAT normalized=True dimension=3
    normal               count=36 type=GL_FLOAT normalized=True dimension=3
    texture_coordinate   count=6  type=GL_FLOAT normalized=True dimension=2
    color                count=8  type=GL_SHORT normalized=True dimension=4
    
#data count=197 interleaved=false  // New line for each attribute data
    -1.000000 1.000000 1.000000 0.000000 1.000000 -1.0000 -1.000000 1.000000\n  
    -1.000000 1.000000 1.000000 0.000000 1.000000 -1.0000 -1.000000 1.000000\n






-- GLOBAL OPTIONS -- 
attributes      : name | name attributes
shape           : TRIANGLES | TRIANGLE_STRIP | QUADS | POINTS | LINES
interleaved     : True | False
indexed         : True | False

"""


def run(file_path, options):

    output_directory = options['o']
    source_directory = options['d']

    with open(file_path) as obj_file:
        source = obj_file.readlines()

    name = ''
    vertices  = []
    indices   = []
    positions = []
    normals   = []
    texture_coordinates = []

    visited = {}

    index = 0

    for line in source:
        line = line.strip()

        if line.startswith('o'):
            # New object
            name = line[1:].strip()
        elif line.startswith('v '):
            # Vertex position
            position = line.split(' ')[1:]
            positions.append(position)
        elif line.startswith('vt'):
            # Vertex texture coordinate
            coordinate = line.split(' ')[1:]
            texture_coordinates.append(coordinate)
        elif line.startswith('vn'):
            # Vertex normal
            normal = line.split(' ')[1:]
            normals.append(normal)
        elif line.startswith('f'):
            # Faces
            triangle = line.split(' ')[1:]

            for vertex in triangle:
                data = tuple(int(x) if x else 0 for x in vertex.split('/'))
                if data in visited:
                    indices.append(visited[data])
                else:
                    position_index = data[0] - 1  # Obj file is 1-indexed.
                    texture_index  = data[1] - 1  # Obj file is 1-indexed.
                    normal_index   = data[2] - 1  # Obj file is 1-indexed.

                    vertices.extend(
                        (*positions[position_index], *texture_coordinates[texture_index], *normals[normal_index])
                    )
                    indices.append(str(index))  # Using string as it's being saved to a text file.
                    visited[data] = str(index)
                    index += 1

    with open(os.path.join(output_directory, name + '.ted'), 'w') as output_file:
        output_file.write('#NAME ' + str(len(name)) + '\n')
        output_file.write(name + '\n')

        output_file.write('#VERTICES ' + str(len(vertices)) + '\n')
        output_file.write(' '.join(vertices) + '\n')

        output_file.write('#INDICES ' + str(len(indices)) + '\n')
        output_file.write(' '.join(indices) + '\n')

        output_file.write('#LAYOUT ' + ' ... ' + '\n')


def main():

    options = {
        'd': sys.argv[0],
        'o': sys.argv[0]
    }

    file_path = sys.argv[1]

    i = 2
    while i < len(sys.argv):
        argument = sys.argv[i]

        if argument.startswith('-'):
            option_name = argument[1:]
            i += 1
            options[option_name] = sys.argv[i]
        else:
            raise SyntaxError('WRONG!')

        i += 1

    print(options)
    run(file_path, options)


if __name__ == '__main__':
    main()
















