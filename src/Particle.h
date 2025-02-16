#ifndef PARTICLE_H
#define PARTICLE_H

#include "mesh.h"


struct Particle {
    std::shared_ptr<ShaderProgram> shaderProgram;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float maxVelocity = 10.0f;
    float density;
    float pressure;
    float mass = 3.0f / 4.0f * M_PI * pow(0.1f, 3) * 1000.0f;
    int id;
    float radius;
    Mesh mesh;
    int gridIndex = -1;
    bool paused = false;

    static float Radius() {
        return 0.1f;
    }

    Particle(std::shared_ptr<ShaderProgram> shaderProgram, glm::vec3 position, float radius, int id) : 
        shaderProgram(shaderProgram),
        position(position),
        velocity(glm::vec3(0.0f)),
        acceleration(glm::vec3(0.0f, 0.0f, 0.0f)),
        density(0.0f),
        pressure(0.0f),
        radius(radius),
        id(id),
        mesh(SPHERE, shaderProgram) {
            this->mesh.makeSphere(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), radius, 36, 18);
        }

    void setGridIndex(int gridIndex) {
        this->gridIndex = gridIndex;
    }

    int getGridIndex() {
        return this->gridIndex;
    }

    void render(float dt){
        this->mesh.updateModelMatrix(this->position);
        this->mesh.render();
        if (paused) {
            return;
        }
        this->update(dt);
    }

    void update(float dt){
        // clamp velocity
        if (glm::length(this->velocity) > this->maxVelocity) {
            this->velocity = glm::normalize(this->velocity) * this->maxVelocity;
            this->position += this->velocity * dt;
            return;
        }
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
    glm::vec4 color;
    float size;
    Mesh mesh;

    RigidPlane(std::shared_ptr<ShaderProgram> shaderProgram, glm::vec3 position, glm::vec3 u, glm::vec3 v, glm::vec4 color, float size) : 
        shaderProgram(shaderProgram),
        position(position),
        u(u),
        v(v),
        color(color),
        size(size),
        mesh(PLANE, shaderProgram) {
            this->mesh.makePlane(this->u, this->v, this->color, this->size);
            this->mesh.updateModelMatrix(this->position);
        }
    
    void render() {
        this->mesh.render();
    }
};

struct RigidPlaneInvisible {
    glm::vec3 position;
    glm::vec3 u;
    glm::vec3 v;

    RigidPlaneInvisible(glm::vec3 position, glm::vec3 u, glm::vec3 v) :
        position(position),
        u(u),
        v(v) {}
};

#endif // PARTICLE_H