#include "sphSolver.h"

SPHSolver::SPHSolver(std::shared_ptr<std::vector<Particle>> particles, std::shared_ptr<ShaderProgram> shaderProgram) :
        particles(particles),
        shaderProgram(shaderProgram),
        Yplane(shaderProgram, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f),
        Backplane(shaderProgram, glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f),
        Leftplane(shaderProgram, glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f),
        Rightplane(shaderProgram, glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f),
        Frontplane(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        grid(Yplane, Backplane, Leftplane, Rightplane, Frontplane, 2, particles)
    {
        grid.populateGrid();
    }

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

void SPHSolver::addParticle(Particle particle) {
    particles->push_back(particle);
}

void SPHSolver::handleCollisions(){
    handlePlaneCollision();
    handleParticleCollision();
}

void SPHSolver::handlePlaneCollision(){
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
    std::vector<std::vector<int>> *gridList = grid.getGrid();
    for (int index = 0; index < gridList->size(); index++) {
        std::array<int, 26> gridNeighbors = grid.getIndexOfNeighbouringGrids(index);
        std::vector<int> particleIndices = gridList->at(index);
        for (int i = 0; i < gridNeighbors.size(); i++) {
            if (gridNeighbors[i] == -1) {
                continue;
            }
            particleIndices.insert(particleIndices.end(), gridList->at(gridNeighbors[i]).begin(), gridList->at(gridNeighbors[i]).end());
        }

        for (int i = 0; i < particleIndices.size(); i++) {
            for (int j = i + 1; j < particleIndices.size(); j++) {
                Particle &p1 = (*particles)[particleIndices[i]];
                Particle &p2 = (*particles)[particleIndices[j]];

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