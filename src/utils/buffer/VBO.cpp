#include "VBO.h"

VBO::VBO() {
    glGenBuffers(1, &_id);
}

void VBO::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VBO::setBuffer(std::vector<Vertex> vertices, GLenum usage) {
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), usage);
}

void VBO::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

VBO::~VBO() {
    //glDeleteBuffers(1, &_id);
}