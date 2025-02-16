#include "Camera.h"

#include "Camera.h"
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : _position(position),
      _worldUp(up),
      _yaw(yaw),
      _pitch(pitch),
      _front(glm::vec3(0.0f, 0.0f, -1.0f)),
      _movementSpeed(0.5f),
      _mouseSensitivity(0.15f),
      _fov(45.0f),
      _fpsMode(false)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(_position, _position + _front, _up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(_fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::processKeyboardInput(Camera_Movement direction, float deltaTime) {
    float velocity = _movementSpeed * deltaTime;
    
    switch(direction) {
        case FORWARD:
            _position += _front * velocity;
            break;
        case BACKWARD:
            _position -= _front * velocity;
            break;
        case LEFT:
            _position -= _right * velocity;
            break;
        case RIGHT:
            _position += _right * velocity;
            break;
        case UP:
            _position += _worldUp * velocity;
            break;
        case DOWN:
            _position -= _worldUp * velocity;
            break;
    }
}

void Camera::processMouseInput(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= _mouseSensitivity;
    yoffset *= _mouseSensitivity;

    _yaw += xoffset;
    _pitch += yoffset;

    if (constrainPitch) {
        if (_pitch > 89.0f) {
            _pitch = 89.0f;
        }
        if (_pitch < -89.0f) {
            _pitch = -89.0f;
        }
    }
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    _fov -= yoffset;
    if (_fov < 1.0f) {
        _fov = 1.0f;
    }
    if (_fov > 45.0f) {
        _fov = 45.0f;
    }
}

void Camera::updateCameraVectors() {
    // Calculate the new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);
    
    // Recalculate the right and up vectors
    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));
}