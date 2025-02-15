#ifndef SPHSOLVER_H
#define SPHSOLVER_H

#include <array>

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
    bool paused = true;

    static float Radius() {
        return 0.05f;
    }

    Particle(std::shared_ptr<ShaderProgram> shaderProgram, glm::vec3 position, float radius, int id) : 
        shaderProgram(shaderProgram),
        position(position),
        velocity(glm::vec3(0.0f)),
        acceleration(glm::vec3(0.0f, -9.8f, 0.0f)),
        density(0.0f),
        pressure(0.0f),
        radius(radius),
        id(id),
        mesh(SPHERE, shaderProgram) {
            this->mesh.makeSphere(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), radius, 36, 18);
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

class SPHSolver {
private :
    bool paused = false;
    std::vector<Particle> *particles;
    RigidPlane Yplane;
    RigidPlane Backplane;
    RigidPlane Leftplane;
    RigidPlane Rightplane;
    RigidPlaneInvisible Frontplane;
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::vector<std::vector<int>> grid; // maps a grid cell index to a list of particle indices
    int num_cells_x;
    int num_cells_y;
    int num_cells_z;
public :
    SPHSolver(std::vector<Particle> *particles, std::shared_ptr<ShaderProgram> shaderProgram);
    
    void update(float dt);

    void populateGrid();

    void handleCollisions();
    void handlePlaneCollision();
    void handleParticleCollision();

    void addParticle(Particle particle);
    void spawnParticles();

    void unpause();
    void pause();

    int getIndexInGrid(int i, int j, int k, int num_cells_x, int num_cells_y) {
        return i + j * num_cells_x + k * num_cells_x * num_cells_y;
    }

    std::array<int, 3> getGridfromIndex(int index, int num_cells_x, int num_cells_y) {
        int k = index / (num_cells_x * num_cells_y);
        int j = (index - k * num_cells_x * num_cells_y) / num_cells_x;
        int i = index - j * num_cells_x - k * num_cells_x * num_cells_y;
        return {i, j, k};
    }

    std::array<int, 26> getIndexOfNeighbouringGrids(int index, int num_cells_x, int num_cells_y, int num_cells_z) {
        std::array<int, 26> neighbours;
        std::array<int, 3> coords = getGridfromIndex(index, num_cells_x, num_cells_y);
        int n1 = getValidIndexInGrid(coords[0] - 1, coords[1] - 1, coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n2 = getValidIndexInGrid(coords[0], coords[1] - 1, coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n3 = getValidIndexInGrid(coords[0] + 1, coords[1] - 1, coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n4 = getValidIndexInGrid(coords[0] - 1, coords[1], coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n5 = getValidIndexInGrid(coords[0], coords[1], coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n6 = getValidIndexInGrid(coords[0] + 1, coords[1], coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n7 = getValidIndexInGrid(coords[0] - 1, coords[1] + 1, coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n8 = getValidIndexInGrid(coords[0], coords[1] + 1, coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n9 = getValidIndexInGrid(coords[0] + 1, coords[1] + 1, coords[2] - 1, num_cells_x, num_cells_y, num_cells_z);
        int n10 = getValidIndexInGrid(coords[0] - 1, coords[1] - 1, coords[2], num_cells_x, num_cells_y, num_cells_z);
        int n11 = getValidIndexInGrid(coords[0], coords[1] - 1, coords[2], num_cells_x, num_cells_y, num_cells_z);
        int n12 = getValidIndexInGrid(coords[0] + 1, coords[1] - 1, coords[2], num_cells_x, num_cells_y, num_cells_z);
        int n13 = getValidIndexInGrid(coords[0] - 1, coords[1], coords[2], num_cells_x, num_cells_y, num_cells_z);
        int n14 = getValidIndexInGrid(coords[0] + 1, coords[1], coords[2], num_cells_x, num_cells_y, num_cells_z);
        int n15 = getValidIndexInGrid(coords[0] - 1, coords[1] + 1, coords[2], num_cells_x, num_cells_y, num_cells_z);
        int n16 = getValidIndexInGrid(coords[0], coords[1] + 1, coords[2], num_cells_x, num_cells_y, num_cells_z);
        int n17 = getValidIndexInGrid(coords[0] + 1, coords[1] + 1, coords[2], num_cells_x, num_cells_y, num_cells_z);
        int n18 = getValidIndexInGrid(coords[0] - 1, coords[1] - 1, coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        int n19 = getValidIndexInGrid(coords[0], coords[1] - 1, coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        int n20 = getValidIndexInGrid(coords[0] + 1, coords[1] - 1, coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        int n21 = getValidIndexInGrid(coords[0] - 1, coords[1], coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        int n22 = getValidIndexInGrid(coords[0], coords[1], coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        int n23 = getValidIndexInGrid(coords[0] + 1, coords[1], coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        int n24 = getValidIndexInGrid(coords[0] - 1, coords[1] + 1, coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        int n25 = getValidIndexInGrid(coords[0], coords[1] + 1, coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        int n26 = getValidIndexInGrid(coords[0] + 1, coords[1] + 1, coords[2] + 1, num_cells_x, num_cells_y, num_cells_z);
        neighbours = {n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26};
        return neighbours;
    }

    int getValidIndexInGrid(int i, int j, int k, int num_cells_x, int num_cells_y, int num_cells_z) {
        int index = getIndexInGrid(i, j, k, num_cells_x, num_cells_y);
        if (index < 0 || index >= num_cells_x * num_cells_y * num_cells_z) {
            return -1;
        }
        return index;
    }
};

#endif // SPHSOLVER_H