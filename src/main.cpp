#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils/util.h"
#include "utils/ShaderProgram.h"
#include "utils/Camera.h"
#include "utils/buffer/VAO.h"
#include "utils/buffer/VBO.h"
#include "utils/buffer/EBO.h"
#include "mesh.h"
#include "sphSolver.h"

#include <iostream>
#include <memory>

bool paused = true;
bool spawnParticles = false;
bool pKeyPressed = false;
bool fKeyPressed = false;
bool tabKeyPressed = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));

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
    // create 9x9x9 cube of particles
    std::vector<Particle> particles;
    Particle particle1(shaderProgram, glm::vec3(0.0f, 0.0f, 0.0f), 0.1f, 0);
    Particle particle2(shaderProgram, glm::vec3(0.0f, 1.0f, -1.0f), 0.1f, 0);
    Particle particle3(shaderProgram, glm::vec3(2.0f, 1.0f, 0.0f), 0.1f, 0);
    Particle particle4(shaderProgram, glm::vec3(-2.0f, 1.0f, 0.0f), 0.1f, 0);
    Particle particle5(shaderProgram, glm::vec3(2.0f, 0.0f, -1.0f), 0.1f, 0);
    particles.push_back(particle1);
    particles.push_back(particle2);
    particles.push_back(particle3);
    particles.push_back(particle4);
    particles.push_back(particle5);
    SPHSolver sphSolver(&particles, shaderProgram);
    Mesh mesh(MeshType::CUBE, shaderProgram);
    mesh.makeCube(glm::vec3(2.0f, 0.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.1f);


    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 lightPos(0.0f, 2.0f, 1.0f);
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram->use();
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
        shaderProgram->setMat4("view", view);
        shaderProgram->setVec3("lightPos", lightPos);
        shaderProgram->setMat4("projection", projection);
        if (paused) {
            sphSolver.pause();
        } else {
            sphSolver.unpause();
        }
        if (spawnParticles){
            sphSolver.spawnParticles();
        }
        sphSolver.update(0.01f);
        //mesh.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
        spawnParticles = false;
    }
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!pKeyPressed) {
            paused = !paused;
            pKeyPressed = true;
        }
    } else {
        pKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (!tabKeyPressed) {
            spawnParticles = true;
            tabKeyPressed = true;
        }
    } else {
        tabKeyPressed = false;
    }

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
        if (!fKeyPressed) {
            camera.setFPSMode(!camera.getFPSMode());
            if (camera.getFPSMode()) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            fKeyPressed = true;
        }
    } else {
        fKeyPressed = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (camera.getFPSMode()) {
        float lastX = SCR_WIDTH / 2.0f;
        float lastY = SCR_HEIGHT / 2.0f;
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
        camera.processMouseInput(xoffset, yoffset);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}