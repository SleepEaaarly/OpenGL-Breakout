#include "game_object.h"

#include <GLFW/glfw3.h>
#include <vector>

class GameLevel
{
public:
    std::vector<GameObject> Bricks;

    GameLevel() { }
    
    // load level from file
    void Load(const char *file, GLuint levelWidth, GLuint levelHeight);
    // render levels
    void Draw(SpriteRenderer &renderer);
    // check if the level completed
    GLboolean IsCompleted();

private:
    void init(std::vector<std::vector<GLuint> > tileData, GLuint levelWidth, GLuint levelHeight);
};

