#version 330 core
layout(location = 0) in vec3 aPos;

uniform vec3 position;
uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(aPos.x + position.x, aPos.y + position.y, aPos.z + position.z, 1.0);
}