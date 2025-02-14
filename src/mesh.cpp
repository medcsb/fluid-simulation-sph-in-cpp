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

    _modelMatrix = glm::mat4(1.0);

    init(_vertices);
}

Mesh::Mesh(MeshType meshType, std::shared_ptr<ShaderProgram> shaderProgram) :
    _shaderProgram(shaderProgram) {
        _vao = VAO();
        _vbo = VBO();
        _ebo = EBO();

        _vao.bind();
        _vbo.bind();
        _ebo.bind();

        _modelMatrix = glm::mat4(1.0);
    }

void Mesh::makeCube(glm::vec3 position, glm::vec3 color, float size) {
    _vertexPositions = {
        glm::vec3(-size / 2, -size / 2, size / 2) + position,
        glm::vec3(size / 2, -size / 2, size / 2) + position,
        glm::vec3(size / 2, size / 2, size / 2) + position,
        glm::vec3(-size / 2, size / 2, size / 2) + position,
        glm::vec3(-size / 2, -size / 2, -size / 2) + position,
        glm::vec3(size / 2, -size / 2, -size / 2) + position,
        glm::vec3(size / 2, size / 2, -size / 2) + position,
        glm::vec3(-size / 2, size / 2, -size / 2) + position
    };
    _vertexColors = {
        color,
        color,
        color,
        color,
        color,
        color,
        color,
        color
    };
    _vertexNormals = {
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    _triangleIndices = {
        glm::uvec3(0, 1, 2),
        glm::uvec3(2, 3, 0),
        glm::uvec3(1, 5, 6),
        glm::uvec3(6, 2, 1),
        glm::uvec3(7, 6, 5),
        glm::uvec3(5, 4, 7),
        glm::uvec3(4, 0, 3),
        glm::uvec3(3, 7, 4),
        glm::uvec3(4, 5, 1),
        glm::uvec3(1, 0, 4),
        glm::uvec3(3, 2, 6),
        glm::uvec3(6, 7, 3)
    };

    for (size_t i = 0; i < _vertexPositions.size(); i++) {
        Vertex vertex;
        vertex.position = _vertexPositions[i];
        vertex.normal = _vertexNormals[i];
        vertex.color = _vertexColors[i];
        _vertices.push_back(vertex);
    }

    _vbo.setBuffer(_vertices);
    _ebo.setBuffer(_triangleIndices);

    init(_vertices);
}

void Mesh::makeSphere(glm::vec3 position, glm::vec3 color, float radius, int sectorCount, int stackCount) {
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2 - (i * glm::pi<float>() / stackCount);
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;
            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);

            Vertex vertex;
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = glm::normalize(vertex.position);
            vertex.color = color;
            _vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0)
                _triangleIndices.push_back(glm::uvec3(k1, k2, k1 + 1));

            if (i != (stackCount - 1))
                _triangleIndices.push_back(glm::uvec3(k1 + 1, k2, k2 + 1));
        }
    }

    _vbo.setBuffer(_vertices);
    _ebo.setBuffer(_triangleIndices);

    init(_vertices);
}

void Mesh::makePlane(glm::vec3 position, glm::vec3 u, glm::vec3 v, glm::vec3 color, float size) {
    // k = sqrt((sqrt(size)) / 2 * (u.x * v.x + u.y * v.y + u.z * v.z))
    float k = sqrt((sqrt(size) / 2) * ((u.x + v.x)*(u.x + v.x) + (u.y + v.y)*(u.y + v.y) + (u.z + v.z)*(u.z + v.z)));
    glm::vec3 a = position + k * u + k * v;
    glm::vec3 b = position + k * u - k * v;
    glm::vec3 c = position - k * u - k * v;
    glm::vec3 d = position - k * u + k * v;

    glm::vec3 normal = glm::normalize(glm::cross(b - a, d - a));

    _vertices = {
        Vertex(a, color, normal),
        Vertex(b, color, normal),
        Vertex(c, color, normal),
        Vertex(d, color, normal)
    };

    _triangleIndices = {
        glm::uvec3(0, 1, 2),
        glm::uvec3(2, 3, 0)
    };

    _vbo.setBuffer(_vertices);
    _ebo.setBuffer(_triangleIndices);

    init(_vertices);

    _modelMatrix = glm::mat4(1.0);

    updateModelMatrix(position);
}


void Mesh::init(std::vector<Vertex> vertices) {
    _vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position)); // Position
    //param 1 : ith layout basicaly the ith vertex attribute (0 for position, 1 for color, 2 for normal)
    //param 2 : 1 vertex has 3 floats
    //param 6 : u need to skip a vertex worth of data to reach the next position related data
    //param 7 : offset of the data in the vertex struct (how many bytes from the start of the struct)
    _vao.linkAttrib(_vbo, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color)); //
    _vao.linkAttrib(_vbo, 2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal)); // 

    _vbo.unbind();
    _vao.unbind();
    _ebo.unbind();
}

void Mesh::updateModelMatrix(glm::vec3 position) {
    _modelMatrix = glm::mat4(1.0);
    _modelMatrix = glm::translate(_modelMatrix, position);
}

void Mesh::render() {
    _shaderProgram->use();
    _shaderProgram->setMat4("model", _modelMatrix);
    _vao.bind();
    glDrawElements(GL_TRIANGLES, _triangleIndices.size() * 3, GL_UNSIGNED_INT, 0);
}

Mesh::~Mesh() {
    _vao.~VAO();
    _vbo.~VBO();
    _ebo.~EBO();
}