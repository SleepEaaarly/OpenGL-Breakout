#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture2D
{
public:
    GLuint ID;
    GLuint Width, Height;
    // texture format
    GLuint Internal_format; // texture store format
    GLuint Image_format;    // input image format
    // texture configuration
    GLuint Wrap_s;          // wrap mode on s axis
    GLuint Wrap_t;          // wrap mode on t axis
    GLuint Filter_min;      // filter mode if texture pixels < screen pixels
    GLuint Filter_max;      // filter mode if texture pixels > screen pixels

    Texture2D(/* args */);
    // Generates texture from image data
    void Generate(GLuint width, GLuint height, unsigned char* data);
    // Binds this texture as current active GL_TEXTURE_2D
    void Bind() const;
};

#endif