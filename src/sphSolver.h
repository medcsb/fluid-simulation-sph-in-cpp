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
    float mass = 0.1f;
    Mesh mesh;

    Particle(std::shared_ptr<ShaderProgram> shaderProgram, glm::vec3 position, int id) : 
        shaderProgram(shaderProgram),
        position(position),
        velocity(glm::vec3(0.0f)),
        acceleration(glm::vec3(0.0f, -0.0001f, 0.0f)),
        density(0.0f),
        pressure(0.0f),
        id(id),
        mesh(SPHERE, shaderProgram) {
            this->mesh.makeSphere(this->position, glm::vec3(0.0f, 0.0f, 1.0f), 0.1f, 36, 18);
        }

    void render(float dt){
        this->mesh.render();
        this->update(dt);
        this->mesh.updateModelMatrix(this->position);
        std::cout << "position : " << this->position.x << " " << this->position.y << " " << this->position.z << std::endl;
    }

    void update(float dt){
        this->velocity += this->acceleration * dt;
        this->position += this->velocity * dt;
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
    
    void render(){
        this->mesh.render();
    }

};

#endif // SPHSOLVER_H