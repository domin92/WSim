#ifndef MASTER_HPP
#define MASTER_HPP

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

class Master{

	int grid_size;
	int node_size;
	int proc_count;
    int node_volume;

	char** mapped_buffer;
	char* main_buffer;

	void receive_from_nodes();
	void send_to_nodes();

    int screen_size;
    GLFWwindow *window;

    void load_shaders();
    unsigned int shaderProgram;
    int positionUniformLocation;

    float squareVertices[12] = {
        0.1f, 0.1f, 0.0f,
        0.1f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.1f, 0.0f,
    };

    unsigned int squareIndices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    void load_buffers();
    unsigned int VAO, VBO, EBO;

public:

	Master(int proc_count, int grid_size, int node_size);
	~Master();

	void main();

};

#endif