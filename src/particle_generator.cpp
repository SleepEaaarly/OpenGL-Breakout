#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(Shader &shader, Texture2D &texture, GLuint amount) {
    this->shader = shader;
    this->texture = texture;
    this->amount = amount;
}

void ParticleGenerator::Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset) {
    GLuint nr_new_particles = 2;
    // Add new particles
    for (int i = 0; i < nr_new_particles; i++) {
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

    for (int i = 0; i < amount; i++) {
        particles.push_back(Particle());
    }
}


