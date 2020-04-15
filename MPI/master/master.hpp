#ifndef MASTER_HPP
#define MASTER_HPP

class Master{

	int grid_size;
	int node_size;
	int proc_count;

	char** tab;

	char* all_tab;

	void receive_from_nodes();
	void send_to_nodes();

public:

	Master(int proc_count, int grid_size, int node_size);
	~Master();

	void main();

};

#endif