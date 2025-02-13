#version 330 core

in vec3 vertexColor; // Receive color from vertex shader
in vec3 vertexNormal;
in vec3 fragPosition;

uniform vec3 lightPos;

out vec4 FragColor;  // Output color

void main() {
    vec3 normal = normalize(vertexNormal);

    // Calculate ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vertexColor;
    
    // Calculate diffuse lighting
    vec3 lightDir = normalize(lightPos - gl_FragCoord.xyz);  // Direction from fragment to light
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vertexColor;
    
    // Combine results
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
