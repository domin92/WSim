#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

uniform vec3 currPosition;
uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 ourColor;
out vec2 TexCoord;

void main() {
    gl_Position = transform * projection * view * model * vec4(aPos.x + currPosition.x, aPos.y + currPosition.y, aPos.z, 1.0);
    ourColor = aColor;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}