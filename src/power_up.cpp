#include "power_up.h"

PowerUp::PowerUp(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, Texture2D &texture) : 
    GameObject(position, SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
