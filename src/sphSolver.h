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
    float pressureConstant = 0.00001f;
    float restDensity = 630.0f;
    float gasConstant = 288.0f;
    float nearGasConstant = 2.15f;
    float collisionDamping = 0.95f;
    float viscosityConstant = 0.00001f;
    float effectLength = 1.3f * Particle::Radius();
public :
    SPHSolver(std::shared_ptr<std::vector<Particle>> particles, std::shared_ptr<ShaderProgram> shaderProgram);
    
    void update(float dt);

    void handleCollisions();
    void handlePlaneCollision();
    void handleParticleCollision();

    void computeForces(int particleIndex, std::vector<int> &neighbours);

    void addParticle(Particle particle);
    void spawnParticles();

    void unpause();
    void pause();

    float smoothingFunction(float r);
};

#endif // SPHSOLVER_H