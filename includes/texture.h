//
// Created by Ted Klein Bergman on 2019-01-10.
//
#pragma once

#define  STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "gl_debug.h"


struct ImageData
{
    // STATIC
    enum Channels { SOURCE = 0, GREY = 1, GREY_ALPHA = 2, RGB = 3, RGBA = 4 };
    
    static ImageData create(std::string path, Channels desired_channels = Channels::SOURCE)
    {
        //       1           grey
        //       2           grey, alpha
        //       3           red, green, blue
        //       4           red, green, blue, alpha
        
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, desired_channels);

        if (data == nullptr)
            throw std::runtime_error("Couldn't load texture " + path + ".");

        if (desired_channels != 0)
            channels = desired_channels;

        return ImageData(data, static_cast<unsigned>(width), static_cast<unsigned>(height), static_cast<unsigned>(channels), std::move(path));
    }

    // Something we can do, but then we have to think about copying and referencing and stuff.
    // static std::unordered_map<std::string, ImageData*> loaded;


    // INSTANCE
    unsigned char* data;
    unsigned width, height, channels;
    const std::string path;

    explicit ImageData(unsigned char* data, unsigned width, unsigned height, unsigned channels, std::string path)
        : data(data), width(width), height(height), channels(channels), path(std::move(path))
    {}
    ~ImageData() { stbi_image_free(data); }


    // TODO(ted): Fix so it works with all channels!!!
    unsigned char at(unsigned row, unsigned column, unsigned channel = 0) const
    {
        assert(row < height && column < width && channel < channels);
        unsigned index = row * width + column;
        return data[index * channels + channel];
    }
};


struct Texture
{
    // STATIC
    enum Type { UNKNOWN, NONE, DIFFUSE, SPECULAR, EMISSION, NORMAL };

    static Texture create(ImageData image, Type type = NONE)
    {
        GLuint id;
        GLCALL(glGenTextures(1, &id));
        GLCALL(glBindTexture(GL_TEXTURE_2D, id));

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GLCALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.5f));

        // #define GL_RED 0x1903
        // #define GL_GREEN 0x1904
        // #define GL_BLUE 0x1905
        // #define GL_ALPHA 0x1906
        // #define GL_RGB 0x1907
        // #define GL_RGBA 0x1908

        assert(image.channels == 4 || image.channels == 3);  // TODO(ted): FIX!!!
        GLenum format = image.channels == 4 ? GL_RGBA : GL_RGB;

        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, std::move(image.data)));
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

        return {id, type, std::move(image.path)};
    }


    // INSTANCE
    GLuint id;
    Type   type;
    std::string path;
};





GLuint load_texture(const std::string& path, bool flip = false)
{
    stbi_set_flip_vertically_on_load(flip);

    ImageData image = ImageData::create(path);

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

    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data));
    GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

    // stbi_image_free(image);
    // Gets freed by destructor.

    return texture;
}



GLuint load_texture(const ImageData& image)
{
    GLuint texture;
    GLCALL(glGenTextures(1, &texture));
    GLCALL(glBindTexture(GL_TEXTURE_2D, texture));

    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLCALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.5f));

    // #define GL_RED 0x1903
    // #define GL_GREEN 0x1904
    // #define GL_BLUE 0x1905
    // #define GL_ALPHA 0x1906
    // #define GL_RGB 0x1907
    // #define GL_RGBA 0x1908

    assert(image.channels == 4 || image.channels == 3);  // TODO(ted): FIX!!!
    GLenum format = image.channels == 4 ? GL_RGBA : GL_RGB;

    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data));
    GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

    return texture;
}


GLuint load_3D_texture(const std::vector<std::string>& faces)
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
        }
        else
        {
            throw std::runtime_error("Couldn't load texture at path " + faces[i] + ".");
        }
        stbi_image_free(data);
    }
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    return texture;
}
