#ifndef NODE_HPP
#define NODE_HPP

class Node{
	int rank;
	int grid_size;
	int node_size;

	int **tab[2]; // Two 2D arrays - input, output
	int current_tab_idx; // Index of the current input tab

	int *share_output_column;
	int *share_input_column;

	int row;
	int col;

	int *output_send_tab;

	void iter();
	void share();
	void send_to_master();
public:

	Node(int rank, int grid_size, int node_size);
	~Node();

	void main();

};

#endif