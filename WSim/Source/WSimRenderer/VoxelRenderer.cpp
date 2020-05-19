#include "VoxelRenderer.h"

#include <fstream>
#include <iterator>

VoxelRenderer::VoxelRenderer(VoxelRendererCallbacks &callbacks, int nodeSizeInVoxels, int gridSizeInNodes, int screenSize, char **voxelBuffers)
    : Renderer(GLFW_OPENGL_CORE_PROFILE, screenSize, screenSize),
      callbacks(callbacks),
      nodeSizeInVoxels(nodeSizeInVoxels),
      gridSizeInNodes(gridSizeInNodes),
      screenSize(screenSize),
      voxelBuffers(voxelBuffers),
      voxelSize(2.0f / (nodeSizeInVoxels * gridSizeInNodes)),
      mvp(createMvp(screenSize)) {
    
    loadBuffers();
    loadShaders();

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);
}

VoxelRenderer::~VoxelRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

glm::mat4 VoxelRenderer::createMvp(int screenSize) {
    const glm::mat4 projection = glm::perspective(glm::radians(80.0f), (float)screenSize / (float)screenSize, 0.1f, 100.0f);
    const glm::mat4 view = glm::lookAt(
        glm::vec3(1.25, 1.25, 2.0f),      // Camera is at (4,3,3), in World Space
        glm::vec3(0.5f, 0.5f, 0.5f),          // and looks at the origin
        glm::vec3(0, 1, 0)           // Head is up (set to 0,-1,0 to look upside-down)
    );
    const glm::mat4 model = glm::mat4(1.0f);
    const glm::mat4 mvp = projection * view * model;
    return mvp;
}

void VoxelRenderer::loadBuffers() {
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

    glEnable(GL_BLEND);                                //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
    glEnable(GL_CULL_FACE);

}

void VoxelRenderer::loadShaders() {
    OGL::Shader vertexShader = OGL::createShaderFromFile(GL_VERTEX_SHADER, "Vertex/VoxelVertex.glsl");
    OGL::Shader fragmentShader = OGL::createShaderFromFile(GL_FRAGMENT_SHADER, "Fragment/VoxelFragment.glsl");
    this->shaderProgram = OGL::createShaderProgram(vertexShader, fragmentShader);
    this->mvpUniformLocation = glGetUniformLocation(shaderProgram, "MVP");
    this->simulationSizeUniformLocation = glGetUniformLocation(shaderProgram, "simSize");
}

void VoxelRenderer::processInput(int button, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void VoxelRenderer::update(float deltaTimeSeconds) {
    callbacks.stepSimulation(deltaTimeSeconds);
}

void VoxelRenderer::render() {

    /*const auto gridSizeInVoxels = nodeSizeInVoxels * gridSizeInNodes;
    for (int z = gridSizeInVoxels-1; z >= 0; z--) {
        for (int y = 0; y < gridSizeInVoxels; y++) {
            for (int x = 0; x < gridSizeInVoxels; x++) {

                int z_in_node = z % nodeSizeInVoxels;
                int y_in_node = y % nodeSizeInVoxels;
                int x_in_node = x % nodeSizeInVoxels;

                int z_in_grid = z / nodeSizeInVoxels;
                int y_in_grid = y / nodeSizeInVoxels;
                int x_in_grid = x / nodeSizeInVoxels;

                int nodeIndex = z_in_grid * gridSizeInNodes * gridSizeInNodes + y_in_grid * gridSizeInNodes + x_in_grid;

                float red = (float)((float*)voxelBuffers[nodeIndex])[(z_in_node * nodeSizeInVoxels * nodeSizeInVoxels + y_in_node * nodeSizeInVoxels + x_in_node) * 4];
                float green = (float)((float*)voxelBuffers[nodeIndex])[(z_in_node * nodeSizeInVoxels * nodeSizeInVoxels + y_in_node * nodeSizeInVoxels + x_in_node) * 4 + 1];
                float blue = (float)((float*)voxelBuffers[nodeIndex])[(z_in_node * nodeSizeInVoxels * nodeSizeInVoxels + y_in_node * nodeSizeInVoxels + x_in_node) * 4 + 2];
                
                glUniform3f(positionUniformLocation,
                              (x - gridSizeInVoxels / 2) * voxelSize,
                              (y - gridSizeInVoxels / 2) * voxelSize,
                              -(z - gridSizeInVoxels / 2) * voxelSize);

                glUniform3f(colorUniformLocation, red, green, blue);

                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            }
        }
    }*/

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, nodeSizeInVoxels, nodeSizeInVoxels, nodeSizeInVoxels, 0, GL_RGBA, GL_FLOAT, voxelBuffers[0]);
    glGenerateMipmap(GL_TEXTURE_3D);

    glUniform1i(simulationSizeUniformLocation, nodeSizeInVoxels);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}
