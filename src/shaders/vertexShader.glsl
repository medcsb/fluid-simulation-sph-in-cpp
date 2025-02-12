#version 330 core

layout (location = 0) in vec3 aPos;   // Position attribute
layout (location = 1) in vec4 aColor; // Color attribute
layout (location = 2) in vec3 aNormal; // Normal attribute (not used for now)

out vec4 vertexColor; // Pass color to fragment shader

void main() {
    gl_Position = vec4(aPos, 1.0);
    vertexColor = aColor;
}
