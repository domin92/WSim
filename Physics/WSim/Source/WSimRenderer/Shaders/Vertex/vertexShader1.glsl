#version 330 core
layout (location = 0) in vec3 aPos;
uniform vec3 currPosition;

void main() {
    gl_Position = vec4(aPos.x + currPosition.x, aPos.y + currPosition.y, aPos.z, 1.0);
}