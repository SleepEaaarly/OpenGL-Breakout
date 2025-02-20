#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"

class SpriteRenderer
{
public:
    SpriteRenderer(Shader &shader);
    ~SpriteRenderer();

    void DrawSprite(Texture2D &texture, glm::vec2 position, 
        glm::vec2 scale = glm::vec2(10., 10.), GLfloat rotation = 0.,
        glm::vec3 color = glm::vec3(1.));

private:
    Shader shader;
    GLuint quadVAO;
    GLuint quadVBO;

    void initRenderData();
};



#endif