#include "VolumeRenderer.hpp"

#include <algorithm>
#include <fstream>
#include <iterator>

VolumeRenderer::VolumeRenderer(VolumeRendererCallbacks &callbacks, int nodeSizeInVoxels, int gridSizeInNodes, int screenSize)
    : Renderer(GLFW_OPENGL_CORE_PROFILE, screenSize, screenSize),
      callbacks(callbacks),
      nodeSizeInVoxels(nodeSizeInVoxels),
      gridSizeInNodes(gridSizeInNodes),
      screenSize(screenSize) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    loadBuffers();
    loadShaders();
    updateCameraFront();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    glUseProgram(shaderProgram);
}

VolumeRenderer::~VolumeRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

glm::mat4 VolumeRenderer::createMvp() {
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenSize / (float)screenSize, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        cameraPos,               // Eye position
        cameraPos + cameraFront, // Focus point
        cameraUp                 // Up direction
    );
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    return mvp;
}

void VolumeRenderer::loadBuffers() {
    float cubeVertices[24] = {
        // front
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,
        1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0,
        // back
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0, 1.0, -1.0,
        -1.0, 1.0, -1.0};

    for (int i = 0; i < 24; i++) {
        cubeVertices[i] += 1.0f;
        cubeVertices[i] *= 0.5f;
    }

    unsigned int cubeIndices[36] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glGenTextures(1, &waterTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, waterTexture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void VolumeRenderer::loadShaders() {
    OGL::Shader vertexShader = OGL::createShaderFromFile(GL_VERTEX_SHADER, "Vertex/VolumeVertex.glsl");
    OGL::Shader fragmentShader = OGL::createShaderFromFile(GL_FRAGMENT_SHADER, "Fragment/VolumeFragment.glsl");
    this->shaderProgram = OGL::createShaderProgram(vertexShader, fragmentShader);
    this->mvpVertexUniformLocation = glGetUniformLocation(shaderProgram, "MVP");
    this->mvpFragmentUniformLocation = glGetUniformLocation(shaderProgram, "cameraPosition");
    this->nodeSizeUniformLocation = glGetUniformLocation(shaderProgram, "nodeSize");
    this->gridSizeUniformLocation = glGetUniformLocation(shaderProgram, "gridSize");
}

void VolumeRenderer::updateCameraFront() {
    cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront.y = sin(glm::radians(pitch));
    cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraFront);
    mvpDirty = true;
}

void VolumeRenderer::update(float deltaTimeSeconds) {
    callbacks.stepSimulation(deltaTimeSeconds);
}

void VolumeRenderer::render() {
    if (mvpDirty) {
        mvp = createMvp();
        mvpDirty = false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, waterTexture);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, nodeSizeInVoxels, nodeSizeInVoxels, nodeSizeInVoxels * gridSizeInNodes * gridSizeInNodes * gridSizeInNodes, 0, GL_RED, GL_FLOAT, callbacks.getVolumeBuffers());
    glGenerateMipmap(GL_TEXTURE_3D);

    glUniform1i(nodeSizeUniformLocation, nodeSizeInVoxels);
    glUniform1i(gridSizeUniformLocation, gridSizeInNodes);
    glUniformMatrix4fv(mvpVertexUniformLocation, 1, GL_FALSE, &mvp[0][0]);
    glUniform3f(mvpFragmentUniformLocation, cameraPos.x, cameraPos.y, cameraPos.z);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void VolumeRenderer::processKeyboardInput(int key, int scancode, int action, int mods) {
    float cameraSpeed = 4.0f * getDeltaTime();
    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_KEY_W:
        cameraPos += cameraSpeed * cameraFront;
        mvpDirty = true;
        break;
    case GLFW_KEY_A:
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        mvpDirty = true;
        break;
    case GLFW_KEY_D:
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        mvpDirty = true;
        break;
    case GLFW_KEY_S:
        cameraPos -= cameraSpeed * cameraFront;
        mvpDirty = true;
        break;
    }
}

void VolumeRenderer::processInput(int button, int action, int mods) {
}

void VolumeRenderer::processMouseMove(double inXpos, double inYpos) {

    float xpos = static_cast<float>(inXpos);
    float ypos = static_cast<float>(inYpos);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateCameraFront();
}

void VolumeRenderer::processScroll(double xoffset, double yoffset) {
    fov -= (float)yoffset;
    fov = std::clamp(fov, 1.f, 90.f);
    mvpDirty = true;
}
