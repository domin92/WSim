#version 330 core

in vec3 worldPos;

uniform sampler3D waterTexture;
uniform int simSize;

out vec4 FragColor;

void main() {

    vec3 cameraPosition = vec3(1.25, 1.25, 2.0f);

    vec3 marchingDirection = cameraPosition - worldPos;

    marchingDirection = normalize(marchingDirection);

    vec3 marchingStep = marchingDirection / 60.0f;

    vec3 currentPosition = worldPos;

    int blueSum = 0;

    for (int i = 0; i < simSize; i++) {

        currentPosition = currentPosition - marchingStep;

        if ((currentPosition.x < 0 ) || (currentPosition.y < 0 ) || (currentPosition.z < 0)) {
            break;
        }

        if ((currentPosition.x > 1 ) || (currentPosition.y > 1 ) || (currentPosition.z > 1)) {
            break;
        }
    
        float textureColor = texture(waterTexture, vec3(currentPosition.x, currentPosition.y, 1.0f - currentPosition.z)).r;

        if(textureColor > 0.1f){
            blueSum += 1;
        }

    }

    if (blueSum <= 0) {
        discard;
    }

    float alpha = float(blueSum)/float(simSize);

    FragColor = vec4(0.0f, 0.0f, 1.0f, sqrt(alpha));

};