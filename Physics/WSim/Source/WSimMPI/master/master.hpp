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

	int grid_size;
	int node_size;
	int proc_count;
    int node_volume;
    int full_size;

	char** mapped_buffer;
	char* main_buffer;

	void receive_from_nodes();
	void send_to_nodes();

    int screen_size;
    float pixel_size;
    GLFWwindow *window;

    void load_shaders();
    unsigned int shaderProgram;
    GLuint positionUniformLocation;
    GLuint mvpUniformLocation;
    glm::mat4 mvp;

    void load_buffers();
    unsigned int VAO, VBO, EBO;

public:

	Master(int proc_count, int grid_size, int node_size);
	~Master();

	void main();

};

#endif