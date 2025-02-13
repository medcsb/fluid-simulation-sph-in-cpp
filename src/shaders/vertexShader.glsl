#version 330 core

layout (location = 0) in vec3 aPos;   // Position attribute
layout (location = 1) in vec3 aColor; // Color attribute
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor; // Pass color to fragment shader
out vec3 vertexNormal;
out vec3 fragPosition;

void main() {
    vertexColor = aColor;
    vertexNormal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}