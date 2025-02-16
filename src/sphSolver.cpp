#include "sphSolver.h"
#include <iostream>

SPHSolver::SPHSolver(std::shared_ptr<std::vector<Particle>> particles, std::shared_ptr<ShaderProgram> shaderProgram) :
        particles(particles),
        shaderProgram(shaderProgram),
        Yplane(shaderProgram, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f),
        Backplane(shaderProgram, glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f),
        Leftplane(shaderProgram, glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f),
        Rightplane(shaderProgram, glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f),
        Frontplane(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        grid(Yplane, Backplane, Leftplane, Rightplane, Frontplane, 2, particles)
    {}

void SPHSolver::update(float dt) {
    Yplane.render();
    Backplane.render();
    Leftplane.render();
    Rightplane.render();
    handleCollisions();
    for (Particle &particle : *particles) {
        particle.render(dt);
        grid.recomputeParticleIndex(particle.id, particle.getGridIndex());
    }
    
    //populateGrid();
}

void SPHSolver::computeDensityPressure(int particleIndex, std::vector<int> &neighbours) {
    Particle &particle = (*particles)[particleIndex];
    particle.density = 0.0f;
    for (int i = 0; i < neighbours.size(); i++) {
        if (neighbours[i] == particleIndex) {
            continue;
        }
        Particle &neighbour = (*particles)[neighbours[i]];
        glm::vec3 distance = particle.position - neighbour.position;
        float distanceLength = glm::length(distance);
        if (distanceLength < smoothingLength) {
            particle.density += neighbour.mass * Wpoly6Kernel(distanceLength);
        }
    }
    particle.pressure = gasConstant * (particle.density - restDensity);
}

void SPHSolver::computePressureAndViscosityForces(int particleIndex, std::vector<int> &neighbours) {
    Particle &particle = (*particles)[particleIndex];
    glm::vec3 pressureForce = glm::vec3(0.0f);
    glm::vec3 viscosityForce = glm::vec3(0.0f);
    for (int i = 0; i < neighbours.size(); i++) {
        if (neighbours[i] == particleIndex) {
            continue;
        }
        Particle &neighbour = (*particles)[neighbours[i]];
        glm::vec3 distance = particle.position - neighbour.position;
        float distanceLength = glm::length(distance);
        if (distanceLength < smoothingLength) {
            glm::vec3 gradient = distance / distanceLength;
            pressureForce += -neighbour.mass * (particle.pressure + neighbour.pressure) / (2 * neighbour.density) * gradient * SpikyGradientKernel(particle.position - neighbour.position);
            viscosityForce += viscosity * neighbour.mass * (neighbour.velocity - particle.velocity) / neighbour.density * ViscosityLaplacianKernel(distanceLength);
        }
    }
    particle.acceleration = pressureForce / particle.density + viscosityForce / particle.density + glm::vec3(0.0f, -9.8f, 0.0f);
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
            computeDensityPressure(particleIndices[i], particleIndices);
            computePressureAndViscosityForces(particleIndices[i], particleIndices);
        }
    }
}

void SPHSolver::spawnParticles() {
    for (int i = 0; i < 100; i++) {
        // random number between -1 and 1
        float x = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1-(-1))));
        float y = 1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2-1)));
        float z = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1-(-1))));
        Particle particle(shaderProgram, glm::vec3(x, y, z), 0.05f, particleCount);
        particleCount++;
        particles->push_back(particle);
    }
    int size = particles->size();
}

float SPHSolver::Wpoly6Kernel(float r) {
    if (r >= 0 && r <= smoothingLength) {
        return 315.0f / (64.0f * M_PI * pow(smoothingLength, 9)) * pow(pow(smoothingLength, 2) - pow(r, 2), 3);
    }
    return 0.0f;
}

glm::vec3 SPHSolver::SpikyGradientKernel(glm::vec3 r) {
    float rLength = glm::length(r);
    if (rLength >= 0 && rLength <= smoothingLength) {
        float scalar = -45.0f / (M_PI * pow(smoothingLength, 6)) * pow(smoothingLength - rLength, 2);
        return static_cast<float>(scalar/rLength) * r;
    }
    return glm::vec3(0.0f);
}

float SPHSolver::ViscosityLaplacianKernel(float r) {
    if (r >= 0 && r <= smoothingLength) {
        return 45.0f / (M_PI * pow(smoothingLength, 6)) * (smoothingLength - r);
    }
    return 0.0f;
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