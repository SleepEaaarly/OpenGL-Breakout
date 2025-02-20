#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "texture.h"
#include "sprite_renderer.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class GameObject {
public:
    glm::vec2 Position, Scale, Velocity;
    glm::vec3 Color;
    GLfloat Rotation;
    GLboolean IsSolid;
    GLboolean Destroyed;

    Texture2D Sprite;

    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 scale, Texture2D sprite, glm::vec3 color = glm::vec3(1.), glm::vec2 velocity = glm::vec2(0., 0.));

    virtual void Draw(SpriteRenderer &renderer);
    virtual GLboolean CheckCollision(GameObject &other);
    void PlayerReset(glm::vec2& playPos, const glm::vec2& PLAYER_SIZE);
};


#endif