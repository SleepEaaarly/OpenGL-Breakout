#include "particle_generator.h"
#include <glm/gtc/matrix_transform.hpp>

GLuint newParticles = 2;

ParticleGenerator::ParticleGenerator(Shader &shader, Texture2D &texture, GLuint amount) {
    this->shader = shader;
    this->texture = texture;
    this->amount = amount;
    init();
}

void ParticleGenerator::Update(GLfloat dt, GameObject &object) {
    // Add new particles
    glm::vec2 offset(object.Scale.x / 4., object.Scale.y / 4.);
    for (int i = 0; i < newParticles; i++) {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(particles[unusedParticle], object, offset);
    }
    // Update all particles
    for (Particle &p : particles) {
        p.Life -= dt;
        if (p.Life > 0) {
            p.Position += p.Velocity * dt;
            p.Color.a -= dt * 2.5;
        }
    }
}

void ParticleGenerator::Draw() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader.Use();
    glm::mat4 projection = glm::ortho(0., 800., 600., 0., -1., 1.);
    for (Particle &p : particles) {
        if (p.Life > 0.f) {
            shader.SetVec2("offset", p.Position);
            shader.SetVec4("color", p.Color);
            shader.SetMat4("projection", projection);
            shader.SetInt("sprite", 0);

            texture.Bind();
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init() {
    GLfloat quad[] = {
        0., 0., 0., 0.,
        0., 1., 0., 1.,
        1., 0., 1., 0.,
        0., 1., 0., 1.,
        1., 1., 1., 1.,
        1., 0., 1., 0.
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (int i = 0; i < amount; i++) {
        particles.push_back(Particle());
    }
}

GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle() {
    // search from last used particle
    for (int i = lastUsedParticle; i < amount; i++) {
        if (particles[i].Life <= 0.f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise do linear search
    for (int i = 0; i < lastUsedParticle; i++) {
        if (particles[i].Life <= 0.f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // Override first particle if others alive
    lastUsedParticle = 0;
    return 0;
}


void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset) {
    GLfloat random = ((rand() % 100) - 50) / 10.f;
    GLfloat rColor = 0.5 + ((rand() % 100) / 100.f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.f);
    particle.Life = 1.f;
    particle.Velocity = -object.Velocity * 0.1f;
}
