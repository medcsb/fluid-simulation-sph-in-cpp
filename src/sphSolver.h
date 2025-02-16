#ifndef SPHSOLVER_H
#define SPHSOLVER_H

#include <array>

#include "mesh.h"
#include "utils/ShaderProgram.h"
#include "grid.h"
#include "particle.h"

class SPHSolver {
private :
    bool paused = false;
    std::shared_ptr<std::vector<Particle>> particles;
    RigidPlane Yplane;
    RigidPlane Backplane;
    RigidPlane Leftplane;
    RigidPlane Rightplane;
    RigidPlaneInvisible Frontplane;
    std::shared_ptr<ShaderProgram> shaderProgram;
    Grid grid;
    unsigned int particleCount = 0;
    float restDensity = 630.0f;
    float gasConstant = 288.0f;
    float nearGasConstant = 2.15f;
    float collisionDamping = 0.95f;
    float viscosityConstant = 0.00001f;
    float smoothingLength = 2.0f * Particle::Radius();
public :
    SPHSolver(std::shared_ptr<std::vector<Particle>> particles, std::shared_ptr<ShaderProgram> shaderProgram);
    
    void update(float dt);

    void handleCollisions();
    void handlePlaneCollision();
    void handleParticleCollision();

    void computeDensityPressure(int particleIndex, std::vector<int> &neighbours);
    void computePressureAndViscosityForces(int particleIndex, std::vector<int> &neighbours);

    void addParticle(Particle particle);
    void spawnParticles();

    void unpause();
    void pause();

    float Wpoly6Kernel(float r);
    float WspikyKernel(float r);
    glm::vec3 GradientSpikyKernel(glm::vec3 rij, float distance);
    float LaplacianViscosityKernel(float r);
};

#endif // SPHSOLVER_H