// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
// clang-format on

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

std::string loadShader(std::string path) {
    std::ifstream ifs(path);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    return content;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
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

    std::string shaderCode3 = loadShader("..\\..\\..\\..\\..\\WSim\\Source\\WSimRenderer\\Shaders\\Fragment\\fragmentShader2.glsl");
    const GLchar *fragmentShaderSource2 = shaderCode3.c_str();

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

    unsigned int fragmentShader1;
    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader1);
    glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int fragmentShader2;
    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader2);
    glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    unsigned int shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);
    glGetShaderiv(shaderProgram1, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    unsigned int shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader1);

    //VAO - stores vertex attribute configuration and knows which VBO to use
    // VAO - Vertax Array Object 1
    float vertices_1[] = {
        0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        -0.5f, 0.0f, 0.0f};

    unsigned int VAO_1, VBO_1;
    glGenVertexArrays(1, &VAO_1);
    glGenBuffers(1, &VBO_1);
    glBindVertexArray(VAO_1);

    //binding actual data to buffer (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO_1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);

    //letting shader know the structure of vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //------------------------------------------------------------------------------
    // VAO - Vertax Array Object 2     -   VBO x2 and EBO x1
    float vertices_ebo[] = {
        0.2f, 0.2f, 0.0f,  // top right
        0.2f, 0.0f, 0.0f,  // bottom right
        0.0f, 00.0f, 0.0f, // bottom left
        0.0f, 0.2f, 0.0f   // top left
    };
    unsigned int indices_ebo[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    float vertices_2[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f};

    unsigned int VAO_2;
    glGenVertexArrays(1, &VAO_2);
    glBindVertexArray(VAO_2);

    //square
    unsigned int VBO_2;

    glGenBuffers(1, &VBO_2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_ebo), vertices_ebo, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_ebo), indices_ebo, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // second triangle

    unsigned int VBO_3;
    glGenBuffers(1, &VBO_3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //triangle #1
        glUseProgram(shaderProgram1);
        glBindVertexArray(VAO_1);
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        //triangle #2
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO_2);
        //glDrawArrays(GL_TRIANGLES, 4, 3);

        //square #1
        glUseProgram(shaderProgram1);
        glBindVertexArray(VAO_2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO_1);
    glDeleteBuffers(1, &VBO_1);
    glfwTerminate();

    return 0;
}
