#version 330 core

in vec3 worldPos;

uniform sampler3D waterTexture;
uniform int nodeSize;
uniform int gridSize;
uniform vec3 cameraPosition;


out vec4 FragColor;

vec3 getWaterTextureCoords(vec3 readPos){
    // This function converts world space coorinates to water texture coordinates. It is needed to properly read multi-node textures.

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

    vec3 marchingDirection = normalize(worldPos - cameraPosition);

    vec3 marchingStep = marchingDirection / float(nodeSize * gridSize);

    vec3 currentPosition = worldPos;

    float blueSum = 0;

    for (int i = 0; i < nodeSize * gridSize * 3 / 2; i++) {

        currentPosition = currentPosition + marchingStep;

        if ((currentPosition.x < 0 ) || (currentPosition.y < 0 ) || (currentPosition.z < 0)) {
            break;
        }

        if ((currentPosition.x > 1 ) || (currentPosition.y > 1 ) || (currentPosition.z > 1)) {
            break;
        }
    
        vec3 readPos = vec3(currentPosition.x, currentPosition.y, currentPosition.z); // Flip Y and Z axis

        readPos = getWaterTextureCoords(readPos);

        vec3 textureColor = texture(waterTexture, readPos).rgb;

        //FragColor = vec4(textureColor, 1); // Uncomment to render only sides with full color
        //return;

        blueSum += textureColor.b;

    }

    if (blueSum <= 1.0f) {
        blueSum = pow(blueSum,4);
    }

    float waterTransparency = sqrt(min(blueSum / 15.0f, 1.0f));

    vec3 waterColor = vec3(20, 160, 235) / 255.0f;

    FragColor = vec4(waterColor, waterTransparency);

};