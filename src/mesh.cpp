#include "Mesh.h"

#include <iostream>

Mesh::Mesh(std::vector<glm::vec3> vertexPositions, 
    std::vector<glm::vec3> vertexColors,
    std::vector<glm::uvec3> triangleIndices,
    std::shared_ptr<ShaderProgram> shaderProgram) :
    _vertexPositions(vertexPositions),
    _vertexColors(vertexColors),
    _triangleIndices(triangleIndices),
    _shaderProgram(shaderProgram) {
    for (size_t i = 0; i < vertexPositions.size(); i++) {
        Vertex vertex;
        vertex.position = vertexPositions[i];
        vertex.color = vertexColors[i];
        _vertices.push_back(vertex);
    }
    _vao = VAO();
    _vao.bind();
    
    _vbo = VBO();
    _vbo.bind();
    _vbo.setBuffer(_vertices);
    
    _ebo = EBO();
    _ebo.bind();
    _ebo.setBuffer(triangleIndices);
    
    init(_vertices);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<glm::uvec3> triangleIndices, std::shared_ptr<ShaderProgram> shaderProgram) :
    _vertices(vertices),
    _triangleIndices(triangleIndices),
    _shaderProgram(shaderProgram) {
    _vao = VAO();
    _vbo = VBO();
    _ebo = EBO();

    _vao.bind();
    _vbo.bind();
    _ebo.bind();

    _vbo.setBuffer(vertices);
    _ebo.setBuffer(triangleIndices);

    init(_vertices);
}

void Mesh::init(std::vector<Vertex> vertices) {
    _vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position)); // Position
    //param 1 : ith layout basicaly the ith vertex attribute (0 for position, 1 for color, 2 for normal)
    //param 2 : 1 vertex has 3 floats
    //param 6 : u need to skip a vertex worth of data to reach the next position related data
    //param 7 : offset of the data in the vertex struct (how many bytes from the start of the struct)
    _vao.linkAttrib(_vbo, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color)); // 

    _vbo.unbind();
    _vao.unbind();
    _ebo.unbind();
}

void Mesh::render() {
    _shaderProgram->use();
    _vao.bind();
    glDrawElements(GL_TRIANGLES, _triangleIndices.size() * 3, GL_UNSIGNED_INT, 0);
}

Mesh::~Mesh() {
    _vao.~VAO();
    _vbo.~VBO();
    _ebo.~EBO();
}