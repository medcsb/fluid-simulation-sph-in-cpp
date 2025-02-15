cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(FLUID_SIMULATION_CPP)

# Define source files
set(SOURCES
    src/main.cpp  # Adjust the path if needed
    src/mesh.cpp
    src/sphSolver.cpp
    src/utils/ShaderProgram.cpp
    src/utils/util.cpp
    src/utils/Camera.cpp
    src/utils/buffer/EBO.cpp
    src/utils/buffer/VBO.cpp
    src/utils/buffer/VAO.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add GLAD source file
target_sources(${PROJECT_NAME} PRIVATE ./lib/glad/src/glad.c)  # Specify the target ${PROJECT_NAME}

# Include directories for GLAD and GLFW
target_include_directories(${PROJECT_NAME} PRIVATE 
  ${CMAKE_SOURCE_DIR}/lib/glad/include # Specify the path to the GLAD include directory
  ${CMAKE_SOURCE_DIR}/lib/glfw/include # Specify the path to the GLFW include directory
  ${CMAKE_SOURCE_DIR}/lib/glm          # Specify the path to the GLM include directory
)

# Link GLFW library
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/glfw/lib/libglfw3.a)
