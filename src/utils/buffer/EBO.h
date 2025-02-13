#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>

#include <vector>
#include <glm/glm.hpp>

#include "VBO.h"

class EBO {

public:
    EBO();
    ~EBO();

    void bind();
    void setBuffer(std::vector<glm::uvec3> indices, GLenum usage = GL_STATIC_DRAW);
    void setBuffer(std::vector<GLuint> indices, GLenum usage = GL_STATIC_DRAW);
    void setBuffer(std::vector<GLint> indices, GLenum usage = GL_STATIC_DRAW);
    void unbind();

private:
    GLuint _id;
};

#endif // EBO_H