#include "VoxelRenderer.h"

#include <fstream>
#include <iterator>

VoxelRenderer::VoxelRenderer(AbstractSimulation &simulation, int nodeSizeInVoxels, int gridSizeInNodes, int screenSize, char **voxelBuffers)
    : Renderer(GLFW_OPENGL_CORE_PROFILE, screenSize, screenSize),
      simulation(simulation),
      nodeSizeInVoxels(nodeSizeInVoxels),
      gridSizeInNodes(gridSizeInNodes),
      screenSize(screenSize),
      voxelBuffers(voxelBuffers),
      voxelSize(2.0f / (nodeSizeInVoxels * gridSizeInNodes)),
      mvp(createMvp(screenSize)) {
    loadBuffers();
    loadShaders();

    glUseProgram(shaderProgram);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
        glm::vec3(1.5f, 1.5f, 2.5f), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0),          // and looks at the origin
        glm::vec3(0, 1, 0)           // Head is up (set to 0,-1,0 to look upside-down)
    );
    const glm::mat4 model = glm::mat4(1.0f);
    const glm::mat4 mvp = projection * view * model;
    return mvp;
}

std::string VoxelRenderer::loadShader(const std::string &path) {
    std::ifstream ifs(path);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    return content;
}

void VoxelRenderer::loadBuffers() {
    float cubeVertices[24] = {
        1.0, -1.0, -1.0,
        1.0, -1.0, 1.0,
        -1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0,
        1.0, 1.0, -1.0,
        1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0};

    for (int i = 0; i < 24; i++) {
        cubeVertices[i] += 1.0f;
        cubeVertices[i] *= 0.5f;
        cubeVertices[i] *= voxelSize;
    }

    unsigned int cubeIndices[36] = {
        1, 2, 4,
        2, 3, 4,
        5, 8, 6,
        8, 7, 6,
        3, 7, 4,
        7, 8, 4,
        1, 5, 2,
        5, 6, 2,
        5, 1, 8,
        1, 4, 8,
        2, 6, 3,
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
}

void VoxelRenderer::loadShaders() {
    int success;
    char infoLog[512];

    std::string shaderCode = loadShader("..\\..\\..\\..\\WSim\\Source\\WSimMPI\\Shaders\\Vertex.glsl");
    const char *vertexShaderSource = shaderCode.c_str();

    std::string shaderCode2 = loadShader("..\\..\\..\\..\\WSim\\Source\\WSimMPI\\Shaders\\Fragment.glsl");
    const GLchar *fragmentShaderSource = shaderCode2.c_str();

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    positionUniformLocation = glGetUniformLocation(shaderProgram, "position");
    mvpUniformLocation = glGetUniformLocation(shaderProgram, "MVP");
}

void VoxelRenderer::processInput(int button, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void VoxelRenderer::update(float dt) {
    simulation.stepSimulation(dt);
}

void VoxelRenderer::render() {
    const auto gridSizeInVoxels = nodeSizeInVoxels * gridSizeInNodes;
    for (int z = 0; z < gridSizeInVoxels; z++) {
        for (int y = 0; y < gridSizeInVoxels; y++) {
            for (int x = 0; x < gridSizeInVoxels; x++) {

                int z_in_node = z % nodeSizeInVoxels;
                int y_in_node = y % nodeSizeInVoxels;
                int x_in_node = x % nodeSizeInVoxels;

                int z_in_grid = z / nodeSizeInVoxels;
                int y_in_grid = y / nodeSizeInVoxels;
                int x_in_grid = x / nodeSizeInVoxels;

                int nodeIndex = z_in_grid * gridSizeInNodes * gridSizeInNodes + y_in_grid * gridSizeInNodes + x_in_grid;

                int power = voxelBuffers[nodeIndex][z_in_node * nodeSizeInVoxels * nodeSizeInVoxels + y_in_node * nodeSizeInVoxels + x_in_node];

                if (power > 0) {
                    glUniform3f(positionUniformLocation,
                                (x - gridSizeInVoxels / 2) * voxelSize,
                                (y - gridSizeInVoxels / 2) * voxelSize,
                                (z - gridSizeInVoxels / 2) * voxelSize);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                }
            }
        }
    }
}
