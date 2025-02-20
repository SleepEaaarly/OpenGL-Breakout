#include "ball_object.h"

BallObject::BallObject() 
    : GameObject(), Radius(12.5f), Stuck(true) { }

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(2 * radius, 2 * radius), sprite, glm::vec3(1.), velocity), Radius(radius), Stuck(true) { }

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width) {
    if (!Stuck) {
        Position += Velocity * dt;
        if (Position.x <= 0.) {
            Position.x = 0.f;
            Velocity.x = -Velocity.x;
        } else if (Position.x + Scale.x >= window_width) {
            Position.x = window_width - Scale.x;
            Velocity.x = -Velocity.x;
        } 
        if (Position.y <= 0.) {     // top
            Position.y = 0.;
            Velocity.y = -Velocity.y;
        }
    }
    return Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity) {
    Position = position;
    Velocity = velocity;
    Stuck = true;
}
