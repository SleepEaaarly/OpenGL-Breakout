#ifndef POWER_UP
#define POWER_UP

#include "game_object.h"

const glm::vec2 SIZE(60, 20);
const glm::vec2 VELOCITY(0.f, 150.f);

class PowerUp : public GameObject
{
private:
    /* data */
public:
    std::string Type;
    GLfloat Duration;
    GLboolean Activated;

    PowerUp(std::string type, glm::vec3 color, GLfloat duration,
            glm::vec2 position, Texture2D &texture);
};

#endif