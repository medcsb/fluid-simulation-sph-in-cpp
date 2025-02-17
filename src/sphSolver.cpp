#include "sphSolver.h"
#include <iostream>

SPHSolver::SPHSolver(std::shared_ptr<std::vector<Particle>> particles, std::shared_ptr<ShaderProgram> shaderProgram) :
        particles(particles),
        shaderProgram(shaderProgram),
        Yplane(shaderProgram, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f),
        Backplane(shaderProgram, glm::vec3(0.0f, 1.0f, -5.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f),
        Leftplane(shaderProgram, glm::vec3(-10.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f),
        Rightplane(shaderProgram, glm::vec3(10.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f),
        Frontplane(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        grid(Yplane, Backplane, Leftplane, Rightplane, Frontplane, 2, particles)
    {}

void SPHSolver::update(float dt) {
    Yplane.render();
    Backplane.render();
    Leftplane.render();
    Rightplane.render();
    for (Particle &particle : *particles) {
        particle.render(dt);
        grid.recomputeParticleIndex(particle.id, particle.getGridIndex());
    }
    handleCollisions();
    
    //populateGrid();
}

void SPHSolver::addParticle(Particle particle) {
    particles->push_back(particle);
}

void SPHSolver::handleCollisions(){
    handlePlaneCollision();
    handleParticleCollision();
}

void SPHSolver::handlePlaneCollision(){
    // Check for collision with planes
    for (Particle &particle : *particles) {
        if (particle.position.y < Yplane.position.y + particle.getRadius()) {
            particle.position.y = Yplane.position.y + particle.getRadius();
            if (particle.velocity.y < 0) {
                particle.velocity.y = -particle.velocity.y * 0.5f;
            }
        }
        if (particle.position.z < Backplane.position.z + particle.getRadius()) {
            particle.position.z = Backplane.position.z + particle.getRadius() + 0.01f;
            if (particle.velocity.z < 0) {
                particle.velocity.z = -particle.velocity.z * 0.5f;
            }
        }
        if (particle.position.z > Frontplane.position.z - particle.getRadius()) {
            particle.position.z = Frontplane.position.z - particle.getRadius() - 0.01f;
            if (particle.velocity.z > 0) {
                particle.velocity.z = -particle.velocity.z * 0.5f;
            }
        }
        if (particle.position.x < Leftplane.position.x + particle.getRadius()) {
            particle.position.x = Leftplane.position.x + particle.getRadius() + 0.01f;
            if (particle.velocity.x < 0) {
                particle.velocity.x = -particle.velocity.x * 0.5f;
            }
        }
        if (particle.position.x > Rightplane.position.x - particle.getRadius()) {
            particle.position.x = Rightplane.position.x - particle.getRadius() - 0.01f;
            if (particle.velocity.x > 0) {
                particle.velocity.x = -particle.velocity.x * 0.5f;
            }
        }
    }
}

void SPHSolver::handleParticleCollision() {
    if (particles->size() == 0) {
        return;
    }
    std::vector<std::vector<int>> *gridMap = grid.getGrid();
    for (int index = 0; index < gridMap->size(); index++) {
        std::vector<int> particleIndices = gridMap->at(index);
        const std::array<int, 26> &neighbours = grid.getNeighbours(index);
        for (int i = 0; i < neighbours.size(); i++) {
            if (neighbours[i] == -1) {
                continue;
            }
            particleIndices.insert(particleIndices.end(), gridMap->at(neighbours[i]).begin(), gridMap->at(neighbours[i]).end());
        }

        for (int i = 0; i < particleIndices.size(); i++) {
            //computeDensityPressure(particleIndices[i], particleIndices);
            computeForces(particleIndices[i], particleIndices);
        }
    }
}

void SPHSolver::computeForces(int particleIndex, std::vector<int> &neighbours) {
    Particle &particle = (*particles)[particleIndex];
    glm::vec3 repulsiveForce = glm::vec3(0.0f);
    glm::vec3 pressureForce = glm::vec3(0.0f);
    for (int i = 0; i < neighbours.size(); i++) {
        if (neighbours[i] == particleIndex) {
            continue;
        }
        Particle &neighbour = (*particles)[neighbours[i]];
        glm::vec3 np = particle.position - neighbour.position; // vector from neighbour to particle
        float distance = glm::length(np);
        glm::vec3 normal = glm::normalize(np);
        pressureForce += -normal * pressureConstant * smoothingFunction(distance);
        //particle.velocity += pressureForce;
        //neighbour.velocity -= pressureForce;
        if (distance < 2 * Particle::Radius()) {
            float overlap = 2 * Particle::Radius() - distance;
            particle.position += overlap * 0.5f * normal;
            neighbour.position -= overlap * 0.5f * normal;
            float relativeVelocity = glm::dot(particle.velocity - neighbour.velocity, normal);
            if (relativeVelocity < 0) {
                repulsiveForce = relativeVelocity * normal;
                particle.velocity -= relativeVelocity * 0.5f * normal;
                neighbour.velocity += relativeVelocity * normal;;
            }
        }
    }
}

float SPHSolver::smoothingFunction(float r) {
    return pow(1 - r/effectLength, 3);
}

void SPHSolver::spawnParticles() {
    for (int i = 0; i < 100; i++) {
        // random number between -1 and 1
        //float x = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1-(-1))));
        //float y = 1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2-1)));
        //float z = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1-(-1))));
        //Particle particle(shaderProgram, glm::vec3(x, y, z), 0.1f, particleCount);
        //particleCount++;
        //particles->push_back(particle);
    }
    //Particle particle(shaderProgram, glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, particleCount);
    //particleCount++;
    //Particle particle2(shaderProgram, glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, particleCount);
    //particleCount++;
    //Particle particle3(shaderProgram, glm::vec3(0.0f, 1.0f, 0.1f), 0.1f, particleCount);
    //particleCount++;
    //particles->push_back(particle);
    //particles->push_back(particle2);
    //particles->push_back(particle3);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 5; k++) {
                Particle particle(shaderProgram, glm::vec3(i * 2 * Particle::Radius(), 1.0f + j * 2 * Particle::Radius(), -1.0f + k * 2 * Particle::Radius()), 0.1f, particleCount);
                particleCount++;
                particles->push_back(particle);
            }
        }
    }
}

void SPHSolver::pause() {
    if (paused) {
        return;
    }
    for (Particle &particle : *particles) {
        particle.pause();
    }
    paused = true;
}

void SPHSolver::unpause() {
    if (!paused) {
        return;
    }
    for (Particle &particle : *particles) {
        particle.unpause();
    }
    paused = false;
}