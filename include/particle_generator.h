#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include "shader.h"
#include "texture.h"
#include "game_object.h"

struct Particle
{
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    GLfloat Life;

    Particle()
        : Position(0.f), Velocity(0.f), Color(1.f), Life(0.f) { }
};

class ParticleGenerator
{
public:
    // Constructor
    ParticleGenerator(Shader &shader, Texture2D &texture, GLuint amount);
    // Update all particles
    void Update(GLfloat dt, GameObject &object);
    // Render all particles
    void Draw();
private:
    std::vector<Particle> particles;
    GLuint amount;

    Shader shader;
    Texture2D texture;
    GLuint VAO;
    GLuint VBO;

    void init();
    GLuint firstUnusedParticle();
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.f));
};

#endif
