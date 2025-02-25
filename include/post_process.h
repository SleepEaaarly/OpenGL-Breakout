#include "shader.h"
#include "texture.h"

class PostProcess
{
private:
    Shader shader;
    GLuint width;
    GLuint height;
    GLboolean chaos;
    GLboolean confuse;
    GLboolean shake;

    Texture2D texture;
    GLuint FBO;
    GLuint MSFBO;
    GLuint RBO;
    GLuint VAO;
    GLuint VBO;

    void initRenderData();
    
public:
    PostProcess(Shader &shader, int width, int height);
    void BeginRender();
    void EndRender();
    void Render(GLfloat time);
};
