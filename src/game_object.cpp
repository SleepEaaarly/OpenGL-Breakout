#include "game_object.h"

GameObject::GameObject():
    Position(0, 0), Scale(1, 1), Velocity(0.), Color(1.), Rotation(0.), IsSolid(false), Destroyed(false), Sprite() { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 scale, Texture2D sprite, glm::vec3 color, glm::vec2 velocity):
    Position(pos), Scale(scale), Velocity(velocity), Color(color), Rotation(0.), IsSolid(false), Destroyed(false), Sprite(sprite) { }

void GameObject::Draw(SpriteRenderer &renderer) {
    if (!Destroyed)
        renderer.DrawSprite(Sprite, Position, Scale, Rotation, Color);
}

GLboolean GameObject::CheckCollision(GameObject &other) {   // AABB collision
    bool collisionX = Position.x + Scale.x >= other.Position.x 
                        && other.Position.x + other.Scale.x >= Position.x;
    bool collisionY = Position.y + Scale.y >= other.Position.y 
                        && other.Position.y + other.Scale.y >= Position.y;
    
    return collisionX && collisionY;
}

void GameObject::PlayerReset(glm::vec2& playPos, const glm::vec2& PLAYER_SIZE) {
    Position = playPos;
    Scale = PLAYER_SIZE;
}