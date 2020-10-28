#version 330 core

in vec3 worldPos;

uniform sampler3D volume;
uniform vec3 cameraPosition;

out vec4 color;

bool isSamplePositionValid(vec3 samplePosition) {
    return samplePosition.x >= 0 && samplePosition.x <= 1
        && samplePosition.y >= 0 && samplePosition.y <= 1
        && samplePosition.z >= 0 && samplePosition.z <= 1;
}

void main(void) {
    vec3 marchingRay = cameraPosition - worldPos;
    marchingRay = normalize(marchingRay);
    vec3 step = marchingRay / 50;
    vec3 samplePosition = worldPos;

    // First step
    float levelSetSign = sign(texture(volume, samplePosition).r);
    samplePosition += step;

    // Next steps
    for (float i = 0; i < 1000; i++) {
        if (!isSamplePositionValid(samplePosition)) {
            break;
        }

        float currentSign = sign(texture(volume, samplePosition).r);
        if (levelSetSign != currentSign) {
            color = vec4(0.0f, 0.0f, 1.f, 1.0f);
            return;
        }

        levelSetSign = currentSign;
        samplePosition += step;
    }

    // Not found water
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
