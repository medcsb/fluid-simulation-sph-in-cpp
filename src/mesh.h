#ifndef MESH_H
#define MESH_H

#include "utils/buffer/VAO.h"
#include "utils/buffer/VBO.h"
#include "utils/buffer/EBO.h"
#include "utils/ShaderProgram.h"
#include "utils/Camera.h"

#include <vector>

class Mesh {

public:

    Mesh(std::vector<glm::vec3> vertexPositions,
         std::vector<glm::vec3> vertexColors,
         std::vector<glm::uvec3> triangleIndices,
         std::shared_ptr<ShaderProgram> shaderProgram);

    Mesh(std::vector<Vertex> vertecies, std::vector<glm::uvec3> triangleIndices, std::shared_ptr<ShaderProgram> shaderProgram);

    ~Mesh();

    void init(std::vector<Vertex> vertices);

    void render();

    std::vector<glm::vec3> &vertexPositions() { return _vertexPositions; }
    std::vector<glm::vec3> &vertexColors() { return _vertexColors; }
    std::vector<glm::uvec3> &triangleIndices() { return _triangleIndices; }

    void setVertexPositions(const std::vector<glm::vec3> &vertexPositions) { _vertexPositions = vertexPositions; }
    void setVertexColors(const std::vector<glm::vec3> &vertexColors) { _vertexColors = vertexColors; }
    void setTriangleIndices(const std::vector<glm::uvec3> &triangleIndices) { _triangleIndices = triangleIndices; }

private:

    std::shared_ptr<ShaderProgram> _shaderProgram;
    std::vector<glm::vec3> _vertexPositions;
    std::vector<glm::vec3> _vertexColors;
    std::vector<glm::uvec3> _triangleIndices;

    std::vector<Vertex> _vertices;

    VAO _vao;
    VBO _vbo;
    EBO _ebo;
};
#endif // MESH_H