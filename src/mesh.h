#ifndef MESH_H
#define MESH_H

#include "utils/buffer/VAO.h"
#include "utils/buffer/VBO.h"
#include "utils/buffer/EBO.h"
#include "utils/ShaderProgram.h"
#include "utils/Camera.h"

#include <vector>

enum MeshType {
    CUBE,
    SPHERE,
    PLANE
};

class Mesh {

public:

    Mesh(std::vector<glm::vec3> vertexPositions,
         std::vector<glm::vec3> vertexColors,
         std::vector<glm::uvec3> triangleIndices,
         std::shared_ptr<ShaderProgram> shaderProgram);

    Mesh(std::vector<Vertex> vertecies, std::vector<glm::uvec3> triangleIndices, std::shared_ptr<ShaderProgram> shaderProgram);

    Mesh(MeshType meshType, std::shared_ptr<ShaderProgram> shaderProgram);

    ~Mesh();

    void init(std::vector<Vertex> vertices);

    void render();

    void updateModelMatrix(glm::vec3 position);
    glm::mat4 getModelMatrix() { return _modelMatrix; }

    void makeCube(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), float size = 1.0f);
    void makeSphere(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
    void makePlane(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 u = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 v = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), float size = 4.0f);

    std::vector<glm::vec3> &vertexPositions() { return _vertexPositions; }
    std::vector<glm::vec3> &vertexNormals() { return _vertexNormals; }
    std::vector<glm::vec3> &vertexColors() { return _vertexColors; }
    std::vector<glm::uvec3> &triangleIndices() { return _triangleIndices; }

    void setVertexPositions(const std::vector<glm::vec3> &vertexPositions) { _vertexPositions = vertexPositions; }
    void setVertexNormals(const std::vector<glm::vec3> &vertexNormals) { _vertexNormals = vertexNormals; }
    void setVertexColors(const std::vector<glm::vec3> &vertexColors) { _vertexColors = vertexColors; }
    void setTriangleIndices(const std::vector<glm::uvec3> &triangleIndices) { _triangleIndices = triangleIndices; }

private:

    std::shared_ptr<ShaderProgram> _shaderProgram;
    std::vector<glm::vec3> _vertexPositions;
    std::vector<glm::vec3> _vertexNormals;
    std::vector<glm::vec3> _vertexColors;
    std::vector<glm::uvec3> _triangleIndices;

    std::vector<Vertex> _vertices;

    glm::mat4 _modelMatrix;

    VAO _vao;
    VBO _vbo;
    EBO _ebo;
};
#endif // MESH_H