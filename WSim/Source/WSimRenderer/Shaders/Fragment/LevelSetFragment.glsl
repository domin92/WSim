#version 330 core

in vec3 worldPos;

uniform sampler3D volume;
uniform vec3 cameraPosition;
uniform int nodeSize;
uniform int gridSize;

out vec4 color;

vec3 getWaterTextureCoords(vec3 readPos) {
    // This function converts world space coorinates to water texture coordinates. It is needed to properly read multi-node textures.

    int z_in_grid = int(readPos.z * float(gridSize));
    int y_in_grid = int(readPos.y * float(gridSize));
    int x_in_grid = int(readPos.x * float(gridSize));

    float nodeOffset = float(z_in_grid * gridSize * gridSize + y_in_grid * gridSize + x_in_grid) / float(gridSize * gridSize * gridSize);

    vec3 waterCoords = readPos;

    waterCoords.x = (readPos.x - float(x_in_grid) / float(gridSize)) * float(gridSize);
    waterCoords.y = (readPos.y - float(y_in_grid) / float(gridSize)) * float(gridSize);
    waterCoords.z = (readPos.z - float(z_in_grid) / float(gridSize)) * float(gridSize);

    waterCoords.z = nodeOffset + waterCoords.z / float(gridSize * gridSize * gridSize);

    return waterCoords;
}

bool isSamplePositionValid(vec3 samplePosition) {
    return samplePosition.x >= 0 && samplePosition.x <= 1 && samplePosition.y >= 0 && samplePosition.y <= 1 && samplePosition.z >= 0 && samplePosition.z <= 1;
}

vec3 lightPos = vec3(-1.0f, 2.0f, -2.0f);
vec3 waterColor = vec3(212.0f / 255.0f, 241.0f / 255.0f, 249.0f / 255.0f); //https://encycolorpedia.com/d4f1f9
float ambientLightPower = 0.25f;

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
                
                if(!isSamplePositionValid(xyzSamplePos)){
                    continue;
                }

                float v = texture(volume, getWaterTextureCoords(xyzSamplePos)).r;

                if(v < 0.0f){
                    x_sum += sign(x) * abs(v);
                    y_sum += sign(y) * abs(v);
                    z_sum += sign(z) * abs(v);
                }
            }
        }
    }

    return normalize(vec3(-x_sum, -y_sum, -z_sum));
}

void main(void) {
    vec3 rayDirection = worldPos - cameraPosition;
    rayDirection = normalize(rayDirection);
    vec3 marchingStep = rayDirection / float(2 * nodeSize * gridSize);
    vec3 samplePosition = worldPos;

    // First step
    samplePosition += marchingStep;

    // Next steps
    for (float i = 0; i < 4 * nodeSize * gridSize; i++) {
        if (!isSamplePositionValid(samplePosition)) {
            break;
        }

        float currentSign = sign(texture(volume, getWaterTextureCoords(samplePosition)).r);

        if (currentSign < 0.0f) {

            vec3 normal = getNormal(samplePosition);
            float reflectpower = 1.0f - dot(normalize(-rayDirection),normalize(normal));
            reflectpower = pow(reflectpower, 1.0f) * 0.5f;

            float lightPower = max(1.0f / distance(lightPos, samplePosition), ambientLightPower);

            vec3 lightPositionNorm = normalize(lightPos - worldPos);
            vec3 viewDir = normalize(cameraPosition - worldPos);
            vec3 reflectDir = reflect(-lightPositionNorm, normal);
            float specularPower = pow(max(dot(viewDir, reflectDir), 0.0), 24) * 0.75f;

            color = vec4(waterColor, reflectpower + specularPower);
            color.rgb *= vec3(lightPower);
            color.rgb += vec3(1.0f) * specularPower;
            return;
        }

        samplePosition += marchingStep;
    }

    // Not found water
    color = vec4(0.9f, 0.9f, 0.9f, 1.0f);
}
