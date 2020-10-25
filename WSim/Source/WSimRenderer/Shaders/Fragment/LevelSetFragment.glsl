#version 330 core

in vec3 worldPos;

uniform sampler3D volume;
uniform int nodeSize;
uniform vec3 cameraPosition;

uniform ivec3 volume_dims;

out vec4 color;

vec3 getWaterTextureCoords(vec3 readPos) {
    int gridSize = 1;

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

void main(void) {
    vec3 marchingRay = cameraPosition - worldPos;
    marchingRay = normalize(marchingRay);
    vec3 step = marchingRay / 50;
    vec3 samplePosition = worldPos;
    float val = 0;
    color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    bool found = false;
    for (float i = 0; i < 1000; i++) {
        if ((samplePosition.x < 0) || (samplePosition.y < 0) || (samplePosition.z < 0)) {
            break;
        }
        if ((samplePosition.x > 1) || (samplePosition.y > 1) || (samplePosition.z > 1)) {
            break;
        }

        val = texture(volume, samplePosition).r;
        if (val < 0.1f) {
            color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
            found = true;
            break;
        }
        samplePosition = samplePosition + step;
    }
    if (!found) {
        color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        //discard;
    }
}
