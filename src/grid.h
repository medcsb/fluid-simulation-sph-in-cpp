#ifndef GRID_H
#define GRID_H

#include "sphSolver.h"
#include "particle.h"

enum direction {
    X,
    Y,
    Z
};

struct Grid {
    std::vector<std::vector<int>> grid; // maps a grid cell index to a list of particle indices
    int num_cells_x;
    int num_cells_y;
    int num_cells_z;
    float Width;
    float Depth;
    float Height;
    float size;
    std::shared_ptr<std::vector<Particle>> particles;
    std::vector<std::array<int, 26>> neighbours;

    std::vector<std::vector<int>>* getGrid() {
        return &grid;
    }

    Grid() {
        particles = nullptr;
        grid = {};
    }

    Grid(RigidPlane Yplane,
         RigidPlane Backplane,
         RigidPlane Leftplane,
         RigidPlane Rightplane,
         RigidPlaneInvisible Frontplane,
         int height,
         std::shared_ptr<std::vector<Particle>> particles) : particles(particles) {
        float r = Particle::Radius();
        size = 2 * r;
        Width = Rightplane.position.x - Leftplane.position.x;
        Depth = Frontplane.position.z - Backplane.position.z;
        Height = height;
        num_cells_x = Width / size;
        num_cells_y = Height / size;
        num_cells_z = Depth / size;
        populateGrid();
        precomputeNeighbours();
    }

    void precomputeNeighbours() {
        neighbours.reserve(num_cells_x * num_cells_y * num_cells_z);
        for (int i = 0; i < num_cells_x * num_cells_y * num_cells_z; i++) {
            neighbours.push_back(getIndexOfNeighbouringGrids(i));
        }
    }

    std::array<int, 26> getNeighbours(int index) {
        return neighbours[index];
    }

    void recomputeParticleIndex(int particle_id, int index) {
        Particle &particle = (*particles)[particle_id];
        int i = (particle.position.x + Width / 2) / size;
        int j = (particle.position.y) / size;
        int k = (particle.position.z + Depth / 2) / size;
        fixIndex(i, X);
        fixIndex(j, Y);
        fixIndex(k, Z);
        int newIndex = getIndexInGrid(i, j, k, num_cells_x, num_cells_y);
        if (index == newIndex) {
            return;
        }
        if (validIndex(index)) {
            removeParticleFromGrid(particle_id, index);
        }
        int sizeofparticles = particles->size();
        grid[newIndex].push_back(particle_id);
        particle.setGridIndex(newIndex);
    }

    bool validIndex(int index) {
        std::array<int, 3> coords = getGridfromIndex(index, num_cells_x, num_cells_y);
        return coords[0] >= 0 && coords[0] < num_cells_x &&
               coords[1] >= 0 && coords[1] < num_cells_y &&
               coords[2] >= 0 && coords[2] < num_cells_z;
    }

    void fixIndex(int &index, direction dir) {
        if (index < 0) {
            index = 0;
        }
        if (dir == X && index >= num_cells_x) {
            index = num_cells_x - 1;
        }
        if (dir == Y && index >= num_cells_y) {
            index = num_cells_y - 1;
        }
        if (dir == Z && index >= num_cells_z) {
            index = num_cells_z - 1;
        }
    }
        

    void populateGrid() {
        int num_cells = num_cells_x * num_cells_y * num_cells_z;
        grid.resize(num_cells);

        if (particles->size() == 0) {
            return;
        }
        

        for (Particle &particle : *particles) {
            float r = Particle::Radius();
            int i = (particle.position.x + Width / 2) / 2 * r;
            int j = (particle.position.y) / 2 * r;
            int k = (particle.position.z + Depth / 2) / 2 * r;
            int index = getIndexInGrid(i, j, k, num_cells_x, num_cells_y);
            grid[index].push_back(particle.id);
            particle.setGridIndex(index);
        }
    }

    int getIndexInGrid(int i, int j, int k, int num_cells_x, int num_cells_y) {
        return i + j * num_cells_x + k * num_cells_x * num_cells_y;
    }

    std::array<int, 3> getGridfromIndex(int index, int num_cells_x, int num_cells_y) {
        int k = index / (num_cells_x * num_cells_y);
        int j = (index - k * num_cells_x * num_cells_y) / num_cells_x;
        int i = index - j * num_cells_x - k * num_cells_x * num_cells_y;
        return {i, j, k};
    }

    void removeParticleFromGrid(int particle_id, int index) {
        for (int i = 0; i < grid[index].size(); i++) {
            if (grid[index][i] == particle_id) {
                grid[index].erase(grid[index].begin() + i);
                break;
            }
        }
    }

    bool inGrid(int particle_id, int index) {
        for (int i = 0; i < grid[index].size(); i++) {
            if (grid[index][i] == particle_id) {
                return true;
            }
        }
        return false;
    }

    std::array<int, 26> getIndexOfNeighbouringGrids(int index) {
        std::array<int, 26> neighbours;
        neighbours.fill(-1);  // Initialize all elements to -1
        
        // Check if input index is valid
        if (index < 0 || index >= num_cells_x * num_cells_y * num_cells_z) {
            return neighbours;
        }
        
        std::array<int, 3> coords = getGridfromIndex(index, num_cells_x, num_cells_y);
        int count = 0;
        
        for (int dx : {-1, 0, 1}) {
            for (int dy : {-1, 0, 1}) {
                for (int dz : {-1, 0, 1}) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;  // Skip the original cell
                    
                    neighbours[count++] = getValidIndexInGrid(
                        coords[0] + dx,
                        coords[1] + dy,
                        coords[2] + dz,
                        num_cells_x,
                        num_cells_y,
                        num_cells_z
                    );
                }
            }
        }
        
        return neighbours;
    }

    int getValidIndexInGrid(int i, int j, int k, int num_cells_x, int num_cells_y, int num_cells_z) {
        // First check if coordinates are within bounds
        if (i < 0 || i >= num_cells_x || 
            j < 0 || j >= num_cells_y || 
            k < 0 || k >= num_cells_z) {
            return -1;
        }
        
        return getIndexInGrid(i, j, k, num_cells_x, num_cells_y);
    }
        
};

#endif // GRID_H