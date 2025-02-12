#include "Mesh.h"

#include <iostream>

Mesh::Mesh(std::vector<glm::vec3> vertexPositions, 
    std::vector<glm::vec4> vertexColors,
    std::vector<glm::vec3> vertexNormals,
    std::vector<glm::uvec3> triangleIndices,
    std::shared_ptr<ShaderProgram> shaderProgram) :
    _vertexPositions(vertexPositions),
    _vertexColors(vertexColors),
    _vertexNormals(vertexNormals),
    _triangleIndices(triangleIndices),
    _shaderProgram(shaderProgram) {
    for (size_t i = 0; i < vertexPositions.size(); i++) {
        Vertex vertex;
        vertex.position = vertexPositions[i];
        vertex.color = vertexColors[i];
        vertex.normal = vertexNormals[i];
        _vertices.push_back(vertex);
    }
    _vao = VAO();
    _vbo = VBO(_vertices);
    _ebo = EBO(triangleIndices);
    init(_vertices);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<glm::uvec3> triangleIndices, std::shared_ptr<ShaderProgram> shaderProgram) :
    _vertices(vertices),
    _triangleIndices(triangleIndices),
    _shaderProgram(shaderProgram) {
    _vao = VAO();
    _vbo = VBO(vertices);
    _ebo = EBO(triangleIndices);
    init(_vertices);
}

void Mesh::init(std::vector<Vertex> vertices) {
    _vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position)); // Position
    //param 1 : ith layout basicaly the ith vertex attribute (0 for position, 1 for color, 2 for normal)
    //param 2 : 1 vertex has 3 floats
    //param 6 : u need to skip a vertex worth of data to reach the next position related data
    //param 7 : offset of the data in the vertex struct (how many bytes from the start of the struct)
    std::cout << "Position offset: " << offsetof(Vertex, position) << std::endl;
    _vao.linkAttrib(_vbo, 1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color)); // 
    std::cout << "Color offset: " << offsetof(Vertex, color) << std::endl;
    _vao.linkAttrib(_vbo, 2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal)); // Normal
    std::cout << "Normal offset: " << offsetof(Vertex, normal) << std::endl;

    _vao.unbind();
    _vbo.unbind();
    _ebo.unbind();
}

void Mesh::render() {
    _shaderProgram->use();
    _vao.bind();
    _ebo.bind();
    glDrawElements(GL_TRIANGLES, _triangleIndices.size() * 3, GL_UNSIGNED_INT, 0);
    std::cout << "Drawing " << _triangleIndices.size() << " triangles" << std::endl;
    _vao.unbind();
}

Mesh::~Mesh() {
    _vao.~VAO();
}