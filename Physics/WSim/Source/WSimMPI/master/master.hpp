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

public:

	Master(int proc_count, int grid_size, int node_size);
	~Master();

	void main();

};

#endif