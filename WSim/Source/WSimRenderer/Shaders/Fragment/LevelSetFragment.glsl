#version 330 core

in vec3 worldPos;

uniform sampler3D volume;
uniform vec3 cameraPosition;

out vec4 color;

bool isSamplePositionValid(vec3 samplePosition) {
    return samplePosition.x >= 0 && samplePosition.x <= 1 && samplePosition.y >= 0 && samplePosition.y <= 1 && samplePosition.z >= 0 && samplePosition.z <= 1;
}

vec3 lightPos = vec3(0.0f, 0.0f, 2.0f);

vec3 getNormal(vec3 samplePosition) {
    float x_sum = 0.0f;
    float y_sum = 0.0f;
    float z_sum = 0.0f;

    // 5x5x5
    for(int x=-2;x<3;x++){
        for(int y=-2;y<3;y++){
            for(int z=-2;z<3;z++){

                vec3 xyzSamplePos = samplePosition;
                xyzSamplePos.x += float(x) / 100.0f;
                xyzSamplePos.y += float(y) / 100.0f;
                xyzSamplePos.z += float(z) / 100.0f;
                        
                float v = texture(volume, xyzSamplePos).r;

                if(v > 0.0f){
                    x_sum += sign(x) * abs(v);
                    y_sum += sign(y) * abs(v);
                    z_sum += sign(z) * abs(v);
                }
            }
        }
    }

    return normalize(vec3(x_sum, y_sum, z_sum));
}

void main(void) {
    vec3 rayDirection = worldPos - cameraPosition;
    rayDirection = normalize(rayDirection);
    vec3 marchingStep = rayDirection / 100;
    vec3 samplePosition = worldPos;

    // First step
    samplePosition += marchingStep;

    float lightPower;
    // Next steps
    for (float i = 0; i < 200; i++) {
        if (!isSamplePositionValid(samplePosition)) {
            break;
        }

        float currentSign = sign(texture(volume, samplePosition).r);

        if (currentSign < 0.0f) {

            vec3 normal = getNormal(samplePosition);
            float reflectpower = 1.0f - dot(normalize(-rayDirection),normalize(normal));
            reflectpower = pow(reflectpower, 1.0f) * 0.5f;

            lightPower = distance(lightPos, samplePosition) / 2.35f;

            color = vec4(0.0f, 0.0f, 0.0f, reflectpower);
            color.rgb = vec3(1.0f - lightPower);
            return;
        }

        samplePosition += marchingStep;
    }

    // Not found water
    color = vec4(0.0f, 0.9f, 0.0f, 1.0f);
}
