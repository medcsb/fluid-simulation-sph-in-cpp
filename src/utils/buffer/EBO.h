#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>

#include <vector>
#include <glm/glm.hpp>

#include "VBO.h"

class EBO {

public:
    EBO() {};
    EBO(GLuint* indices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    EBO(std::vector<GLuint> indices, GLenum usage = GL_STATIC_DRAW);
    EBO(std::vector<glm::uvec3> indices, GLenum usage = GL_STATIC_DRAW);
    EBO(std::vector<Vertex> vertices, GLenum usage = GL_STATIC_DRAW);
    ~EBO();

    void bind();
    void unbind();

private:
    GLuint _id;
};

#endif // EBO_H