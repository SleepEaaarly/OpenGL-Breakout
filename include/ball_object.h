#include "game_object.h"

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

typedef std::tuple<GLboolean, glm::vec2, GLfloat> Collision;

class BallObject : public GameObject
{
public:
    GLfloat Radius;
    GLboolean Stuck;
    GLboolean Sticky;
    GLboolean PassThrough;

    BallObject();
    BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

    glm::vec2 Move(GLfloat dt, GLuint window_width);
    void BallReset(glm::vec2 position, glm::vec2 velocity, const GLfloat radius);

    Collision GetCollision(GameObject &other);
    GLboolean CheckCollision(GameObject &other) override;
//    Collision CollisionPrecise(GameObject &other);    
};