#ifndef SPHSOLVER_H
#define SPHSOLVER_H

#include "mesh.h"
#include "utils/ShaderProgram.h"

struct Particle {
    std::shared_ptr<ShaderProgram> shaderProgram;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float density;
    float pressure;
    int id;
    float radius;
    Mesh mesh;
    bool paused = false;

    Particle(std::shared_ptr<ShaderProgram> shaderProgram, glm::vec3 position, float radius, int id) : 
        shaderProgram(shaderProgram),
        position(position),
        velocity(glm::vec3(0.0f)),
        acceleration(glm::vec3(0.0f, -0.1f, 0.0f)),
        density(0.0f),
        pressure(0.0f),
        radius(radius),
        id(id),
        mesh(SPHERE, shaderProgram) {
            this->mesh.makeSphere(this->position, glm::vec3(0.0f, 0.0f, 1.0f), radius, 36, 18);
        }

    void render(float dt){
        this->mesh.render();
        if (paused) {
            return;
        }
        this->update(dt);
        this->mesh.updateModelMatrix(this->position);
    }

    void update(float dt){
        this->velocity += this->acceleration * dt;
        this->position += this->velocity * dt;
    }

    float getRadius(){
        return this->radius;
    }

    void pause(){
        this->paused = true;
    }

    void unpause(){
        this->paused = false;
    }
};

struct RigidPlane {
    std::shared_ptr<ShaderProgram> shaderProgram;
    glm::vec3 position;
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 color;
    float size;
    Mesh mesh;

    RigidPlane(std::shared_ptr<ShaderProgram> shaderProgram, glm::vec3 position, glm::vec3 u, glm::vec3 v, glm::vec3 color, float size) : 
        shaderProgram(shaderProgram),
        position(position),
        u(u),
        v(v),
        color(color),
        size(size),
        mesh(PLANE, shaderProgram) {
            this->mesh.makePlane(this->position, this->u, this->v, this->color, this->size);
        }
    
    void render() {
        this->mesh.render();
    }
};

class SPHSolver {
private :
    bool paused = false;
    std::vector<Particle> *particles;
    RigidPlane Yplane;
    std::shared_ptr<ShaderProgram> shaderProgram;
public :
    SPHSolver(std::vector<Particle> *particles, std::shared_ptr<ShaderProgram> shaderProgram) :
        particles(particles),
        shaderProgram(shaderProgram),
        Yplane(shaderProgram, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), 2.0f) {}
    
    void update(float dt) {
        Yplane.render();
        for (Particle &particle : *particles) {
            particle.render(dt);
        }

        handleCollisions();

        for (Particle &particle : *particles) {
            particle.mesh.updateModelMatrix(particle.position);
            particle.mesh.render();
        }
    }

    void addParticle(Particle particle) {
        particles->push_back(particle);
    }

    void handleCollisions(){
        handleYPlaneCollision();
        handleParticleCollision();
    }

    void handleYPlaneCollision(){
        for (Particle &particle : *particles) {
            if (particle.position.y < Yplane.position.y + particle.getRadius()) {
                particle.position.y = Yplane.position.y + particle.getRadius();
                if (particle.velocity.y < 0) {
                    particle.velocity.y = -particle.velocity.y * 0.5f;
                }
            }
        }
    }

    void handleParticleCollision() {
        for (size_t i=0; i < particles->size(); i++) {
            for (size_t j=i+1; j < particles->size(); j++) {
                Particle &p1 = (*particles)[i];
                Particle &p2 = (*particles)[j];

                glm::vec3 distance = p1.position - p2.position;
                float distanceLength = glm::length(distance);

                if (distanceLength < 2 * p1.getRadius()) {
                    glm::vec3 normal = glm::normalize(distance);
                    float overlap = 2 * p1.getRadius() - distanceLength;
                    p1.position += overlap / 2 * normal;
                    p2.position -= overlap / 2 * normal;

                    glm::vec3 relativeVelocity = p1.velocity - p2.velocity;
                    float velocityAlongNormal = glm::dot(relativeVelocity, normal);
                    if (velocityAlongNormal > 0) {
                        continue;
                    }

                    glm::vec3 impulse = normal * velocityAlongNormal;
                    p1.velocity -= impulse;
                    p2.velocity += impulse;
                }
            }
        }
    }

    void pause(){
        if (paused) {
            return;
        }
        for (Particle &particle : *particles) {
            particle.pause();
        }
        paused = true;
    }

    void unpause(){
        if (!paused) {
            return;
        }
        for (Particle &particle : *particles) {
            particle.unpause();
        }
        paused = false;
    }

};

#endif // SPHSOLVER_H