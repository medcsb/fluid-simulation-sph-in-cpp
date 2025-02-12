#include "VAO.h"

VAO::VAO() {
    glGenVertexArrays(1, &_id);
}

void VAO::linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLboolean normalized, GLsizei stride, void* offset) {
    VBO.bind();
    glVertexAttribPointer(layout, numComponents, type, normalized, stride, offset);
    glEnableVertexAttribArray(layout);
    VBO.unbind();
}

void VAO::bind() {
    glBindVertexArray(_id);
}

void VAO::unbind() {
    glBindVertexArray(0);
}

VAO::~VAO() {
    glDeleteVertexArrays(1, &_id);
}

