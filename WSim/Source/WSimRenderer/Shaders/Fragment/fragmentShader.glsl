#version 330 core

in vec3 worldPos;

uniform sampler3D volume;
uniform sampler1D colormap;
uniform vec3 cameraPosition;

uniform ivec3 volume_dims;

out vec4 color;

void main(void) {
    vec3 marchingRay = cameraPosition - worldPos;
    marchingRay = normalize(marchingRay);
    vec3 step = marchingRay / 50;
    vec3 samplePosition = worldPos;
    float val = 0;
    for (int i = 0; i < 100; i++) {
        if ((samplePosition.x <  0 )|| (samplePosition.y < 0 )|| (samplePosition.z < 0)) {
            break;
        }
        if ((samplePosition.x > 1 )|| (samplePosition.y  > 1 )|| (samplePosition.z > 1)) {
            break;
        }
        val = val + texture(volume, samplePosition).r;
            
        if (val >= 0.9) {
            color = vec4(0.0f, 0.0f, 0.9f, 1.0f);
            return;
        }
        samplePosition = samplePosition + step;
    }
    if (val < 0.2) {
        discard;
    }
    color = vec4(texture(colormap, val).rgb, val);
}
