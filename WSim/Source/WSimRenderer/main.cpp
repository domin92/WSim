// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <fstream>
#include <algorithm>
// clang-format on

#define WINDOW_WIDTH 640
#define WINDOWHEIGHT 480

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 defaultEye = glm::vec3(0.5, 0.5, 1.5);
glm::vec3 center = glm::vec3(0.5, 0.5, 0.5);
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

std::string loadShader(std::string path) {
    std::ifstream ifs(path);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    return content;
}

void loadVolume(uint8_t *dataBuffer, uint8_t *colorBuffer) {
    std::string pathVolume = "..\\..\\..\\..\\..\\WSim\\Source\\WSimRenderer\\Textures\\neghip_64x64x64_uint8.raw";
    std::string pathColors = "..\\..\\..\\..\\..\\WSim\\Source\\WSimRenderer\\Textures\\samsel-linear-green.png";

    std::ifstream file(pathVolume, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    if (file.read(reinterpret_cast<char *>(dataBuffer), size)) {
    }

    std::ifstream file2(pathColors, std::ios::binary | std::ios::ate);
    std::streamsize size2 = file2.tellg();
    file2.seekg(0, std::ios::beg);
    if (file2.read(reinterpret_cast<char *>(colorBuffer), size2)) {
    }
}

int rendererMain() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOWHEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //SHADERS
    int success;
    char infoLog[512];

    std::string shaderCode = loadShader("..\\..\\..\\..\\..\\WSim\\Source\\WSimRenderer\\Shaders\\Vertex\\vertexShader1.glsl");
    const char *vertexShaderSource = shaderCode.c_str();

    std::string shaderCode2 = loadShader("..\\..\\..\\..\\..\\WSim\\Source\\WSimRenderer\\Shaders\\Fragment\\fragmentShader.glsl");
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
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //------------------------------------------------------------------------------
    // VAO - Vertax Array Object

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};

    glEnable(GL_DEPTH_TEST);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    const int XDIM = 64, YDIM = 64, ZDIM = 64;
    const int size = XDIM * YDIM * ZDIM;
    int volDims[] = {XDIM, YDIM, ZDIM};

    uint8_t dataBuffer[size];
    uint8_t colorMapBuffer[274];

    loadVolume(dataBuffer, colorMapBuffer);
    for (int i = 0; i < size; i++) {

        dataBuffer[i] = 2;
    }
    unsigned int textureVolume, textureColorMap;

    //volume
    glGenTextures(1, &textureVolume);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, textureVolume);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, XDIM, YDIM, ZDIM, 0, GL_RED, GL_UNSIGNED_BYTE, dataBuffer);
    glGenerateMipmap(GL_TEXTURE_3D);

    //colormap
    glGenTextures(1, &textureColorMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, textureColorMap);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 180, 0, GL_RGB, GL_UNSIGNED_BYTE, colorMapBuffer);
    glGenerateMipmap(GL_TEXTURE_1D);

    //glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // vs
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    int eyePosLoc = glGetUniformLocation(shaderProgram, "eye_pos");
    int volumeScaleLoc = glGetUniformLocation(shaderProgram, "volume_scale");

    // fs
    int volumeLoc = glGetUniformLocation(shaderProgram, "volume");
    int colormapLoc = glGetUniformLocation(shaderProgram, "colormap");
    int volumeDimsLoc = glGetUniformLocation(shaderProgram, "volume_dims");
    int dtScaleLoc = glGetUniformLocation(shaderProgram, "dt_scale");

    int longestAxis = std::max(volDims[0], std::max(volDims[1], volDims[2]));
    

    //vs
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(60.0f * 3.14f / 180.0f, 640.0f / 480.0f, 0.1f, 100.0f);

    //ps
    glm::vec3 volScale = glm::vec3(volDims[0] / longestAxis, volDims[1] / longestAxis, volDims[2] / longestAxis);
    float dt_scale = 1.0f; //sampling rate

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // vs
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(eyePosLoc, 1, glm::value_ptr(cameraPos));
        glUniform3fv(volumeScaleLoc, 3, glm::value_ptr(volScale));

        // ps
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, textureVolume);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, textureColorMap);
        glUniform1i(volumeLoc, 0);
        glUniform1i(colormapLoc, 1);
        glUniform3iv(volumeDimsLoc, 3, volDims);
        glUniform1f(dtScaleLoc, dt_scale);


        //drawing
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 39);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();

    return 0;
}
