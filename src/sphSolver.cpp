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
            computeDensityPressure(particleIndices[i], particleIndices);
            computePressureAndViscosityForces(particleIndices[i], particleIndices);
        }
    }
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
            particle.density += neighbour.mass * WspikyKernel(distanceLength);
        }
    }
    particle.pressure = glm::max(0.0f, gasConstant * (particle.density - restDensity));
}

float SPHSolver::Wpoly6Kernel(float r) {
    return 315.0f / (64.0f * M_PI * pow(smoothingLength, 9)) * pow(pow(smoothingLength, 2) - pow(r, 2), 3);
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
        glm::vec3 rij = particle.position - neighbour.position;
        float distance = glm::length(rij);
        if (distance == 0) {
            // make the pressure force go in a random direction by adding a small random value to each component
            continue;
        }
        if (neighbour.density == 0) {
            continue;
        }
        if (distance < smoothingLength) {
            pressureForce += - (neighbour.mass / neighbour.density) * ((particle.pressure + neighbour.pressure) / 2) * GradientSpikyKernel(rij, distance);
            viscosityForce +=  viscosityConstant * (neighbour.mass * (particle.velocity - neighbour.velocity) / neighbour.density) * LaplacianViscosityKernel(distance);
        }
    }
    particle.acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    if (particle.density == 0) {
        return;
    }
    particle.acceleration += (pressureForce) / particle.density;
}

glm::vec3 SPHSolver::GradientSpikyKernel(glm::vec3 rij, float distance) {
    return static_cast<float>(-45.0f / (M_PI * pow(smoothingLength, 6)) * pow(smoothingLength - distance, 2)) * (rij / distance);
}

float SPHSolver::WspikyKernel(float r) {
    return 15.0f / (M_PI * pow(smoothingLength, 6)) * pow(smoothingLength - r, 3);
}

float SPHSolver::LaplacianViscosityKernel(float r) {
    return 45.0f / (M_PI * pow(smoothingLength, 6)) * (smoothingLength - r);
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