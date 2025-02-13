#include "EBO.h"

EBO::EBO() {
    glGenBuffers(1, &_id);
}

void EBO::setBuffer(std::vector<glm::uvec3> indices, GLenum usage) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec3), indices.data(), usage);
}

void EBO::setBuffer(std::vector<GLuint> indices, GLenum usage) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), usage);
}

void EBO::setBuffer(std::vector<GLint> indices, GLenum usage) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), indices.data(), usage);
}

void EBO::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

void EBO::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

EBO::~EBO() {
    //glDeleteBuffers(1, &_id);
}