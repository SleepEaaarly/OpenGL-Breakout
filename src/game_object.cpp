#include "game_object.h"

GameObject::GameObject():
    Position(0, 0), Scale(1, 1), Velocity(0.), Color(1.), Rotation(0.), IsSolid(false), Destroyed(false), Sprite() { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 scale, Texture2D sprite, glm::vec3 color, glm::vec2 velocity):
    Position(pos), Scale(scale), Velocity(velocity), Color(color), Rotation(0.), IsSolid(false), Destroyed(false), Sprite(sprite) { }

void GameObject::Draw(SpriteRenderer &renderer) {
    renderer.DrawSprite(Sprite, Position, Scale, Rotation, Color);
}
