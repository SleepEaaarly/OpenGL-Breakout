#include "ball_object.h"

BallObject::BallObject() 
    : GameObject(), Radius(12.5f), Stuck(true), Sticky(false), PassThrough(false) { }

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(2 * radius, 2 * radius), sprite, glm::vec3(1.), velocity), Radius(radius), Stuck(true), Sticky(false), PassThrough(false) { }

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

void BallObject::BallReset(glm::vec2 position, glm::vec2 velocity, const float radius) {
    Position = position;
    Velocity = velocity;
    Radius = radius;
    Stuck = true;
}

Collision BallObject::GetCollision(GameObject &other) {
    glm::vec2 spherePoint(Position.x + Radius, Position.y + Radius);
    glm::vec2 aabbLeftTop(other.Position.x, other.Position.y);
    glm::vec2 aabbRightBottom(other.Position.x + other.Scale.x, other.Position.y + other.Scale.y);
    
    glm::vec2 nearestPoint = glm::clamp(spherePoint, aabbLeftTop, aabbRightBottom);

    float dist = glm::distance(nearestPoint, spherePoint);

    return std::make_tuple(dist <= Radius, nearestPoint, dist);
}

GLboolean BallObject::CheckCollision(GameObject &other) {
    return std::get<0>(GetCollision(other));
}