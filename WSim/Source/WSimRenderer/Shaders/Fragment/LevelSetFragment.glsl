#version 330 core

in vec3 worldPos;

uniform sampler3D volume;
uniform vec3 cameraPosition;
uniform int nodeSize;
uniform int gridSize;

out vec4 color;

vec3 lightPos = vec3(-1.0f, 2.0f, -2.0f);
vec3 waterColor = vec3(0.0f / 255.0f, 180.0f / 255.0f, 255.0f / 255.0f);
float ambientLightPower = 0.25f;
vec4 wallColor = vec4(0.975f, 0.975f, 0.975f, 1.0f);
vec4 floorColor = vec4(0.95f, 0.95f, 0.95f, 1.0f);
float simSize = float(nodeSize * gridSize);

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

vec3 getNormal(vec3 samplePosition) {
    vec3 normal = vec3(0.0f);

    // 5x5x5
    for(int x=-1;x<=1;x++){
        for(int y=-1;y<=1;y++){
            for(int z=-1;z<=1;z++){

                vec3 xyzSamplePos = samplePosition;
                xyzSamplePos.x += float(x) / simSize;
                xyzSamplePos.y += float(y) / simSize;
                xyzSamplePos.z += float(z) / simSize;
                
                if(!isSamplePositionValid(xyzSamplePos)){
                    continue;
                }

                float v = texture(volume, getWaterTextureCoords(xyzSamplePos)).r;

                if(v < 0.0f){
                    normal.x += sign(x) * abs(v);
                    normal.y += sign(y) * abs(v);
                    normal.z += sign(z) * abs(v);
                }
            }
        }
    }

    return normalize(-normal);
}

void main(void) {
    vec3 rayDirection = normalize(worldPos - cameraPosition);
    vec3 marchingStep = rayDirection / float(2 * simSize);
    vec3 samplePosition = worldPos;

    color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // First step
    samplePosition += marchingStep;
    float lastSign = sign(texture(volume, getWaterTextureCoords(samplePosition)).r);

    if(lastSign < 0.0f){ // Starting in water
        lastSign = sign(1.0f);
    }

    float hitNumber = 0.0f;

    // Next steps
    for (float i = 0; i < 4 * simSize; i++) {
        if (!isSamplePositionValid(samplePosition)) {
            break;
        }

        float currentSign = sign(texture(volume, getWaterTextureCoords(samplePosition)).r);

        if (currentSign!=lastSign){

            hitNumber += 2.0f;

            if (currentSign < 0.0f) { // Air->Water hit
                vec3 normal = getNormal(samplePosition);

                vec3 lightPositionNorm = normalize(lightPos - worldPos);
                vec3 viewDir = normalize(cameraPosition - worldPos);
                vec3 reflectDir = reflect(-lightPositionNorm, normal);
                float specularPower = pow(max(dot(viewDir, reflectDir), 0.0), 24) * 0.75f;
                color.rgb += vec3(1.0f) * specularPower;

                float reflectpower = 1.0f - dot(normalize(-rayDirection),normalize(normal));
                reflectpower = pow(reflectpower, 1.0f) * 0.35f;

                color.rgb += waterColor * reflectpower;
                color.rgb -= vec3(1.0f) * pow(reflectpower, 0.75f);

            } else{ // Water->Air hit
                vec3 normal = getNormal(samplePosition);

                vec3 lightPositionNorm = normalize(lightPos - worldPos);
                vec3 viewDir = normalize(cameraPosition - worldPos);
                vec3 reflectDir = reflect(-lightPositionNorm, normal);
                float specularPower = pow(max(dot(viewDir, reflectDir), 0.0), 24) * 0.75f;
                color.rgb += vec3(1.0f) * specularPower / hitNumber;
            }
        }

        /*if (currentSign < 0.0f) { // Inside water
            color.rgb = (color.rgb + waterColor.rgb) * 0.5f;
        }*/


        /*float currentSign = sign(texture(volume, getWaterTextureCoords(samplePosition)).r);

        if (currentSign < 0.0f) {

            vec3 normal = getNormal(samplePosition);
            float reflectpower = 1.0f - dot(normalize(-rayDirection),normalize(normal));
            reflectpower = pow(reflectpower, 0.75f) * 0.35f;

            float lightPower = max(1.0f / distance(lightPos, samplePosition), ambientLightPower);

            vec3 lightPositionNorm = normalize(lightPos - worldPos);
            vec3 viewDir = normalize(cameraPosition - worldPos);
            vec3 reflectDir = reflect(-lightPositionNorm, normal);
            float specularPower = pow(max(dot(viewDir, reflectDir), 0.0), 24) * 0.75f;

            color = vec4(waterColor, reflectpower + specularPower);
            color.rgb *= vec3(lightPower);
            color.rgb += vec3(1.0f) * specularPower;
            return;
        }*/
        lastSign = currentSign;
        samplePosition += marchingStep;
    }

    // Water not found
    if (samplePosition.y < 0.01f){
        color += floorColor; // Floor
    } else{
        color += wallColor; // Wall
    }
}
