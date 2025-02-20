#include <texture.h>

Texture2D::Texture2D() : 
    Width(0), Height(0), Internal_format(GL_RGB), Image_format(GL_RGB), Wrap_s(GL_REPEAT), Wrap_t(GL_REPEAT), Filter_min(GL_LINEAR_MIPMAP_LINEAR), Filter_max(GL_LINEAR) 
{
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data) {
    Width = width;
    Height = height;
    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, Internal_format, width, height, 0, Image_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_max);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);
}
