#ifndef MASTER_HPP
#define MASTER_HPP

class Master{

	int grid_size;
	int node_size;
	int proc_count;

	int** tab;

	void receive_from_nodes();

public:

	Master(int proc_count, int grid_size, int node_size);
	~Master();

	void main();

};

#endif