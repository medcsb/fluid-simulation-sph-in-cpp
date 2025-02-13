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

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
private:
    bool _fpsMode;
    // Camera attributes
    glm::vec3 _position;
    glm::vec3 _front;
    glm::vec3 _up;
    glm::vec3 _right;
    glm::vec3 _worldUp;
    
    // Euler angles
    float _yaw;
    float _pitch;
    
    // Camera options
    float _movementSpeed;
    float _mouseSensitivity;
    float _fov;

    // Private methods
    void updateCameraVectors();

public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);
    
    // Returns view matrix for rendering
    glm::mat4 getViewMatrix() const;
    
    // Returns projection matrix
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    // Process keyboard input
    void processKeyboardInput(Camera_Movement direction, float deltaTime);
    
    // Process mouse input
    void processMouseInput(float xoffset, float yoffset, bool constrainPitch = true);
    
    // Process mouse scroll
    void processMouseScroll(float yoffset);
    
    // Getters
    glm::vec3 getPosition() const { return _position; }
    glm::vec3 getFront() const { return _front; }
    float getFOV() const { return _fov; }
    bool getFPSMode() const { return _fpsMode; }
    
    // Setters
    void setFPSMode(bool mode) { _fpsMode = mode; }
    void setPosition(const glm::vec3& position) { _position = position; }
};

#endif // CAMERA_H