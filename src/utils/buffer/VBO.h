#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 normal;

    Vertex(glm::vec3 position, glm::vec4 color, glm::vec3 normal) :
        position(position),
        color(color),
        normal(normal) {}
    // default constructor
    Vertex() {}
};

class VBO {

public:
    VBO() {};
    VBO(GLfloat* vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    VBO(std::vector<GLfloat> vertices, GLenum usage = GL_STATIC_DRAW);
    VBO(std::vector<glm::vec3> vertices, GLenum usage = GL_STATIC_DRAW);
    VBO(std::vector<Vertex> vertices, GLenum usage = GL_STATIC_DRAW);
    ~VBO();

    void bind();
    void unbind();

private:
    GLuint _id;
};

#endif // VBO_H