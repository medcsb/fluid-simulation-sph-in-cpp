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
void processInput(GLFWwindow *window);
void processMouseButtonInput(GLFWwindow *window, int button, int action, int mods);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, processMouseButtonInput);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
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
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    };
    
    std::vector<glm::uvec3> indices = {
        glm::uvec3(0, 1, 2)
    };
    Mesh mesh(vertices, indices, shaderProgram);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        mesh.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void processMouseButtonInput(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << "Mouse position: " << xpos << " " << ypos << std::endl;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}