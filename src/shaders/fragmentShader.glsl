#version 330 core

in vec4 vertexColor; // Receive color from vertex shader
out vec4 FragColor;  // Output color

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
