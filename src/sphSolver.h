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
public :
    SPHSolver(std::shared_ptr<std::vector<Particle>> particles, std::shared_ptr<ShaderProgram> shaderProgram);
    
    void update(float dt);

    void handleCollisions();
    void handlePlaneCollision();
    void handleParticleCollision();

    void addParticle(Particle particle);
    void spawnParticles();

    void unpause();
    void pause();
};

#endif // SPHSOLVER_H