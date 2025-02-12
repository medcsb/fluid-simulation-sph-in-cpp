#include "Camera.h"

Camera::Camera(int width,
    int height,
    glm::vec3 position,
    glm::vec3 orientation,
    glm::vec3 up,
    float speed,
    float sensitivity,
    float yaw,
    float pitch,
    bool firstClick) :
    position(position), orientation(orientation), up(up), width(width), height(height), speed(speed), sensitivity(sensitivity), yaw(yaw), pitch(pitch), firstClick(firstClick)
    {
        lastX = width / 2.0f;
        lastY = height / 2.0f;
    }

void Camera::updateMatrix(float fov, float nearPlane, float farPlane) {
    viewMatrix = glm::lookAt(position, position + orientation, up);
    projectionMatrix = glm::perspective(glm::radians(fov), (float)width / (float)height, nearPlane, farPlane);
}

void Camera::setMatrix(ShaderProgram &shaderProgram, const char *uniform) {
    shaderProgram.use();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.getId(), uniform), 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix));
}

void Camera::processInputs(GLFWwindow* window) {
    float deltaTime = 0.0f;
    float currentTime = glfwGetTime();
    float lastTime = 0.0f;
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    float cameraSpeed = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        position += cameraSpeed * orientation;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= cameraSpeed * orientation;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(orientation, up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(orientation, up)) * cameraSpeed;
}

void Camera::mouseMouvement(float xpos, float ypos) {
    if (firstClick) {
        lastX = width / 2.0f;
        lastY = height / 2.0f;
        firstClick = false;
    }

    float xOffset, yOffset;
    xOffset = xpos - lastX;
    yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    orientation = glm::normalize(front);
}