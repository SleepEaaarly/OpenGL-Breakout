#include "sprite_renderer.h"
#include <glm/gtc/matrix_transform.hpp>

extern const GLuint SCR_WIDTH;
extern const GLuint SCR_HEIGHT;

void SpriteRenderer::initRenderData() {
    GLfloat vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0., 0., 0., 0.,
        0., 1., 0., 1.,
        1., 1., 1., 1.,
        1., 0., 1., 0.,
    };

    // generate vao and bind vao
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    // move data into vbo
    // Note: Must bind VAO before bind VBO. Because 执行VAO绑定之后, 其后的所有VBO配置都是这个VAO对象的一部分".
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set vao to tell how to read data from vbo and transfer them into shaders
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);

    // unbind vao and vbo
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpriteRenderer::DrawSprite(Texture2D &texture, glm::vec2 position,
    glm::vec2 scale, GLfloat rotation, glm::vec3 color) 
{
    shader.Use();
    glm::mat4 model = glm::mat4(1.);
    model = glm::translate(model, glm::vec3(position, 0.f));
    
    model = glm::translate(model, glm::vec3(0.5 * scale.x, 0.5 * scale.y, 0));
    model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));
    model = glm::translate(model, glm::vec3(-0.5 * scale.x, -0.5 * scale.y, 0));

    model = glm::scale(model, glm::vec3(scale, 1.0));
    
    glm::mat4 projection = glm::ortho(0., 800., 600., 0., -1., 1.);

    shader.SetMat4("model", model);
    shader.SetMat4("projection", projection);
    shader.SetVec3("spriteColor", color);

    shader.SetInt("image", 0);

    texture.Bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

SpriteRenderer::SpriteRenderer(Shader &shader) {
    this->shader = shader;
    initRenderData();
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}
