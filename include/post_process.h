#ifndef POST_PROCESS
#define POST_PROCESS

#include "shader.h"
#include "texture.h"

class PostProcess
{
private:
    Shader shader;
    GLuint width;
    GLuint height;

    Texture2D texture;
    GLuint FBO;
    GLuint MSFBO;
    GLuint RBO;
    GLuint VAO;
    GLuint VBO;

    void initRenderData();
    
public:
    GLboolean chaos;
    GLboolean confuse;
    GLboolean shake;

    PostProcess(Shader &shader, int width, int height);
    void BeginRender();
    void EndRender();
    void Render(GLfloat time);
};

#endif