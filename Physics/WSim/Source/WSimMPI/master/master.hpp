#ifndef MASTER_HPP
#define MASTER_HPP

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/vec3.hpp"
#include "../glm/mat4x4.hpp"
// clang-format on

class Master{

    // MPI
	int grid_size;
    int node_size;
    int full_size;
	int proc_count;
    int node_volume;

	char** mapped_buffer;
	char* main_buffer;

	void receive_from_nodes();
	void send_to_nodes();

    // OGL
    int screenSize;
    float pixelSize;
    GLFWwindow *window;

    void loadShaders();
    unsigned int shaderProgram;
    GLuint positionUniformLocation;
    GLuint mvpUniformLocation;
    glm::mat4 mvp;

    void loadBuffers();
    unsigned int VAO, VBO, EBO;

public:

	Master(int proc_count, int grid_size, int node_size);
	~Master();

	void main();

};

#endif