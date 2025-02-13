#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils/util.h"
#include "utils/ShaderProgram.h"
#include "utils/Camera.h"
#include "utils/buffer/VAO.h"
#include "utils/buffer/VBO.h"
#include "utils/buffer/EBO.h"
#include "mesh.h"

#include <iostream>
#include <memory>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* createWindow(int width, int height, const char* title) {
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        exitOnCriticalError("Failed to create GLFW window", "createWindow");
        glfwTerminate();
        return NULL;
    }
    return window;
}

GLFWwindow* initGLFWContext() {
    initGLFW();
    GLFWwindow* window = createWindow(SCR_WIDTH, SCR_HEIGHT, "FLUID SIMULATION");
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        exitOnCriticalError("Failed to initialize GLAD", "initGLFWContext");
        glfwTerminate();
        return NULL;
    }
    return window;
}


int main() {
    GLFWwindow* window = initGLFWContext();
    std::shared_ptr<ShaderProgram> shaderProgram = ShaderProgram::genBasicShaderProgram("../src/shaders/vertexShader.glsl", "../src/shaders/fragmentShader.glsl");
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
    };
    
    std::vector<glm::uvec3> indices = {
        glm::uvec3(0, 1, 2)
    };
    Mesh mesh(vertices, indices, shaderProgram);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 model = glm::mat4(1.0f);
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram->use();
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
        shaderProgram->setMat4("model", model);
        shaderProgram->setMat4("view", view);
        shaderProgram->setMat4("projection", projection);
        mesh.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboardInput(FORWARD, 0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboardInput(BACKWARD, 0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboardInput(LEFT, 0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboardInput(RIGHT, 0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.processKeyboardInput(UP, 0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.processKeyboardInput(DOWN, 0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        camera.setFPSMode(!camera.getFPSMode());
        if (camera.getFPSMode()) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (camera.getFPSMode()) {
        float lastX = SCR_WIDTH / 2.0f;
        float lastY = SCR_HEIGHT / 2.0f;
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        camera.processMouseInput(xoffset, yoffset);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}