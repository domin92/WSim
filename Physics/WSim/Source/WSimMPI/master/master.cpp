// clang-format off
#include <fstream>
#include <cstdlib>
#include <mpi.h>
#include <iostream>
#include "Master.hpp"
// clang-format on

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

std::string loadShader(std::string path) {
    std::ifstream ifs(path);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    return content;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

Master::Master(int proc_count, int grid_size, int node_size) {

    this->proc_count = proc_count;
    this->grid_size = grid_size;
    this->node_size = node_size;

    full_size = node_size * grid_size;

    node_volume = node_size * node_size * node_size;

    main_buffer = new char[proc_count * node_volume];

    mapped_buffer = new char *[proc_count - 1];

    for (int i = 0; i < proc_count - 1; i++) {
        mapped_buffer[i] = main_buffer + (i + 1) * node_volume;
    }

    pixel_size = 2.0f / (float)full_size;

    // MVP 
    glm::mat4 Projection = glm::perspective(glm::radians(80.0f), (float)screen_size / (float)screen_size, 0.1f, 100.0f);

    glm::mat4 View = glm::lookAt(
        glm::vec3(1.5f, 1.5f, 2.5f), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0),       // and looks at the origin
        glm::vec3(0, 1, 0)        // Head is up (set to 0,-1,0 to look upside-down)
    );

    glm::mat4 Model = glm::mat4(1.0f);

    mvp = Projection * View * Model;

    // OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    screen_size = 1000;

    window = glfwCreateWindow(screen_size, screen_size, "WSim", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

}

Master::~Master() {
    delete[] mapped_buffer;
    delete[] main_buffer;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();

    MPI_Abort(MPI_COMM_WORLD, 0);
}

void Master::load_shaders() {
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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    positionUniformLocation = glGetUniformLocation(shaderProgram, "position");
    mvpUniformLocation = glGetUniformLocation(shaderProgram, "MVP");
}

void Master::load_buffers() {

    float squareVertices[12] = {
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };

    for (int i = 0; i < 12; i++) {
        squareVertices[i] *= pixel_size;
    }

    unsigned int squareIndices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    float cubeVertices[24] = {
        1.0, -1.0, -1.0,
        1.0, -1.0, 1.0,
        -1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0,
        1.0, 1.0, -1.0,
        1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0
    };

    for (int i = 0; i < 24; i++) {
        cubeVertices[i] += 1.0f;
        cubeVertices[i] *= 0.5f;
        cubeVertices[i] *= pixel_size;
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
        6, 7, 3
    };   

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

void Master::send_to_nodes() {
    MPI_Scatter(main_buffer, node_volume, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Master::receive_from_nodes() {
    MPI_Gather(MPI_IN_PLACE, 0, MPI_CHAR, main_buffer, node_volume, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Master::main() {

    for (int z = 0; z < full_size; z++) {
        for (int y = 0; y < full_size; y++) {
            for (int x = 0; x < full_size; x++) {

                int z_in_node = z % node_size;
                int y_in_node = y % node_size;
                int x_in_node = x % node_size;

                int z_in_grid = z / node_size;
                int y_in_grid = y / node_size;
                int x_in_grid = x / node_size;

                int idx = z_in_grid * grid_size * grid_size + y_in_grid * grid_size + x_in_grid;

                int r = rand() % 100;

                if (r > 40) {
                    mapped_buffer[idx][z_in_node * node_size * node_size + y_in_node * node_size + x_in_node] = 1;
                }
            }
        }
    }

    send_to_nodes();

    load_shaders();

    load_buffers();

    glUseProgram(shaderProgram);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);

    while (!glfwWindowShouldClose(window)) {

        receive_from_nodes();

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        for (int z = 0; z < full_size; z++) {
            for (int y = 0; y < full_size; y++) {
                for (int x = 0; x < full_size; x++) {

                    int z_in_node = z % node_size;
                    int y_in_node = y % node_size;
                    int x_in_node = x % node_size;

                    int z_in_grid = z / node_size;
                    int y_in_grid = y / node_size;
                    int x_in_grid = x / node_size;

                    int idx = z_in_grid * grid_size * grid_size + y_in_grid * grid_size + x_in_grid;

                    int power = mapped_buffer[idx][z_in_node * node_size * node_size + y_in_node * node_size + x_in_node];
                    
                    if (power > 0) {
                        glUniform3f(positionUniformLocation, (x - full_size / 2) * pixel_size, (y - full_size / 2) * pixel_size, (z - full_size / 2) * pixel_size);
                        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                    } 

                }
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

}
