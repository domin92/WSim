#version 330 core
layout(location = 0) in vec3 aPos;


uniform mat4 view;
uniform mat4 projection;
uniform vec3 eye_pos;
uniform vec3 volume_scale;


out vec3 vray_dir;
flat out vec3 transformed_eye;

void main() {
    //gl_Position = projection * view *  vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    //TexCoord = vec2(aTexCoord.x, aTexCoord.y);

    // Translate the cube to center it at the origin.
    vec3 volume_translation = vec3(0.5) - volume_scale * 0.5;
    gl_Position = projection * view * vec4(aPos * volume_scale + volume_translation, 1);

    // Compute eye position and ray directions in the unit cube space
    transformed_eye = (eye_pos - volume_translation) / volume_scale;
    vray_dir = aPos - transformed_eye;
}


//out vec3 vray_dir;
//flat out vec3 transformed_eye;
//
//void main() {
//    // Translate the cube to center it at the origin.
//    vec3 volume_translation = vec3(0.5) - volume_scale * 0.5;
//    gl_Position = projection * view * vec4(pos * volume_scale + volume_translation, 1);
//
//    // Compute eye position and ray directions in the unit cube space
//    transformed_eye = (eye_pos - volume_translation) / volume_scale;
//    vray_dir = pos - transformed_eye;
//};