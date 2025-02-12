#include "VBO.h"

VBO::VBO(GLfloat* vertices, GLsizeiptr size, GLenum usage) {
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VBO::VBO(std::vector<GLfloat> vertices, GLenum usage) {
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), usage);
}

VBO::VBO(std::vector<glm::vec3> vertices, GLenum usage) {
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), usage);
}

VBO::VBO(std::vector<Vertex> vertices, GLenum usage) {
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), usage);
}

void VBO::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VBO::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

VBO::~VBO() {
    glDeleteBuffers(1, &_id);
}