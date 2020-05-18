#version 330 core
layout(location = 0) in vec3 aPos;

uniform vec3 position;
uniform vec3 color;
uniform mat4 MVP;

out vec3 worldPosition;

void main() {
    gl_Position = MVP * vec4(aPos.x + position.x, aPos.y + position.y, aPos.z + position.z, 1.0);
    //worldPosition = vec3(aPos.x + position.x, aPos.y + position.y, aPos.z + position.z);
    worldPosition = color;
}