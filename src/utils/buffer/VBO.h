#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;

    Vertex(glm::vec3 position, glm::vec3 color, glm::vec3 normal) :
        position(position),
        color(color),
        normal(normal) {}
    // default constructor
    Vertex() {}
};

class VBO {

public:
    VBO();
    ~VBO();

    void bind();
    void setBuffer(std::vector<Vertex> vertices, GLenum usage = GL_STATIC_DRAW);
    void unbind();

private:
    GLuint _id;
};

#endif // VBO_H