#include "post_process.h"

PostProcess::PostProcess(Shader &shader, int width, int height) : shader(shader), width(width), height(height), chaos(GL_FALSE), confuse(GL_FALSE), shake(GL_FALSE)
{
    glGenFramebuffers(1, &FBO);
    glGenFramebuffers(1, &MSFBO);
    glGenRenderbuffers(1, &RBO);

    glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    texture.Generate(width, height, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.ID, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    initRenderData();
}

void PostProcess::BeginRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcess::EndRender() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::Render(GLfloat time) {
    shader.Use();
    shader.SetFloat("time", time);
    shader.SetBool("chaos", chaos);
    shader.SetBool("confuse", confuse);
    shader.SetBool("shake", shake);
    shader.SetInt("scene", 0);
    texture.Bind();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcess::initRenderData() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    GLfloat vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0., 0., 0., 0.,
        0., 1., 0., 1.,
        1., 1., 1., 1.,
        1., 0., 1., 0.,
    };
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
