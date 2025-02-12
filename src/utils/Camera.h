#ifndef CAMERA_H
#define CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "ShaderProgram.h"

class Camera {

public:

Camera(int width,
    int height,
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
    float speed = 3.0f,
    float sensitivity = 0.05f,
    float yaw = -90.0f,
    float pitch = 0.0f,
    bool firstClick = true);

    void processInputs(GLFWwindow* window);
    void mouseMouvement(float xpos, float ypos);

    void setMatrix(ShaderProgram &shaderProgram, const char *uniform);
    void updateMatrix(float fov, float nearPlane, float farPlane);

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getOrientation() const { return orientation; }
    glm::vec3 getUp() const { return up; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    float getSpeed() const { return speed; }
    float getSensitivity() const { return sensitivity; }

    void setPosition(const glm::vec3 &position) { this->position = position; }
    void setOrientation(const glm::vec3 &orientation) { this->orientation = orientation; }
    void setUp(const glm::vec3 &up) { this->up = up; }
    void setWidth(int width) { this->width = width; }
    void setHeight(int height) { this->height = height; }
    void setSpeed(float speed) { this->speed = speed; }
    void setSensitivity(float sensitivity) { this->sensitivity = sensitivity; }

private:

    glm::vec3 position;
    glm::vec3 orientation;
    glm::vec3 up;
    
    int width;
    int height;
    
    float speed;
    float sensitivity;

    float yaw;
    float pitch;

    bool firstClick;
    float lastX;
    float lastY;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

#endif // CAMERA_H