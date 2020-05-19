#version 330 core
layout(location = 0) in vec3 pos;


uniform mat4 view;
uniform mat4 projection;

out vec3 worldPos;

void main() {
    gl_Position = projection * view * vec4(pos, 1.0f);
    worldPos = pos;
}
