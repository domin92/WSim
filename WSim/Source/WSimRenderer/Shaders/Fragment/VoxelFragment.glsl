#version 330 core

in vec3 worldPos;

uniform sampler3D waterTexture;
uniform int nodeSize;
uniform int gridSize;

out vec4 FragColor;

vec3 getWaterCoords(vec3 readPos){
    int z_in_grid = int(readPos.z * float(gridSize));
    int y_in_grid = int(readPos.y * float(gridSize));
    int x_in_grid = int(readPos.x * float(gridSize));

    float nodeOffset = float(z_in_grid * gridSize * gridSize + y_in_grid * gridSize + x_in_grid) / float(gridSize * gridSize * gridSize);

    vec3 waterCoords = readPos;

    waterCoords.x = (readPos.x - float(x_in_grid)/float(gridSize)) * float(gridSize);
    waterCoords.y = (readPos.y - float(y_in_grid)/float(gridSize)) * float(gridSize);
    waterCoords.z = (readPos.z - float(z_in_grid)/float(gridSize)) * float(gridSize);

    waterCoords.z = nodeOffset + waterCoords.z / float(gridSize * gridSize * gridSize);

    return waterCoords;
}

void main() {

    vec3 cameraPosition = vec3(1.25, 1.25, 2.0f);

    vec3 marchingDirection = cameraPosition - worldPos;

    marchingDirection = normalize(marchingDirection);

    vec3 marchingStep = marchingDirection / float(nodeSize * gridSize);

    vec3 currentPosition = worldPos;

    int blueSum = 0;

    for (int i = 0; i < nodeSize * gridSize; i++) {

        currentPosition = currentPosition - marchingStep;

        if ((currentPosition.x < 0 ) || (currentPosition.y < 0 ) || (currentPosition.z < 0)) {
            break;
        }

        if ((currentPosition.x > 1 ) || (currentPosition.y > 1 ) || (currentPosition.z > 1)) {
            break;
        }
    
        vec3 readPos = vec3(currentPosition.x, 1.0f - currentPosition.y, 1.0f - currentPosition.z); // Flip Y and Z axis

        readPos = getWaterCoords(readPos);

        vec3 textureColor = texture(waterTexture, readPos).rgb;

        //FragColor = vec4(textureColor, 1);
        //return;

        if(textureColor.b > 0.1f){
            blueSum += 1;
        }

    }

    if (blueSum <= 0) {
        discard;
    }

    float alpha = min(float(blueSum)/ 15.0f, 1.0f);

    FragColor = vec4(0.0f, 0.0f, 1.0f, sqrt(alpha));

};