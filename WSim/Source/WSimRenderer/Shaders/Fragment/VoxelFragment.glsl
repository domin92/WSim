#version 330 core

out vec4 FragColor;

in vec3 worldPosition;

void main() {
    FragColor = vec4(worldPosition, 1.0f);
};