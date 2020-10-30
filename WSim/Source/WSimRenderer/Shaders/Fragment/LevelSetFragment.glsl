#version 330 core

in vec3 worldPos;

uniform sampler3D volume;
uniform vec3 cameraPosition;

out vec4 color;

bool isSamplePositionValid(vec3 samplePosition) {
    return samplePosition.x >= 0 && samplePosition.x <= 1 && samplePosition.y >= 0 && samplePosition.y <= 1 && samplePosition.z >= 0 && samplePosition.z <= 1;
}

vec3 lightPos = vec3(0.0f, 0.0f, 5.0f);

void main(void) {
    vec3 marchingRay = worldPos - cameraPosition;
    marchingRay = normalize(marchingRay);
    vec3 step = marchingRay / 500;
    vec3 samplePosition = worldPos;

    // First step
    samplePosition += step;

    float lightPower;
    // Next steps
    for (float i = 0; i < 1000; i++) {
        if (!isSamplePositionValid(samplePosition)) {
            break;
        }

        float currentSign = sign(texture(volume, samplePosition).r);
        if (currentSign < 0.0f) {
            lightPower = distance(lightPos, samplePosition) / 5.35f;
            color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
            color.b -= lightPower;
            return;
        }

        samplePosition += step;
    }

    // Not found water
    color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
