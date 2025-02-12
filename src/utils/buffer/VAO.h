#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include "VBO.h"

class VAO {

public:
    VAO();

    void linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type = GL_FLOAT, GLboolean normalized = GL_FALSE, GLsizei stride = 0, void* offset = nullptr);
    void bind();
    void unbind();

    ~VAO();

private:
    GLuint _id;
};

#endif // VAO_H