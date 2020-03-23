#ifndef NODE_HPP
#define NODE_HPP

class Node{

	int rank;
	int grid_size;
	int node_size;

	int share_thickness; // Thickness of shared areas

	int **tab[2]; // Two 2D arrays - input, output
	int current_tab_idx; // Index of the current input tab

	// Position in grid
	int row;
	int col;

	// Share arrays
	int sh_horizontal_size;
	int *sh_horizontal_L_in; // Left
	int *sh_horizontal_L_out;
	int *sh_horizontal_R_in; // Right
	int *sh_horizontal_R_out;

	int sh_vertical_size;
	int *sh_vertical_U_in; // Up
	int *sh_vertical_U_out;
	int *sh_vertical_D_in; // Down
	int *sh_vertical_D_out;

	int sh_corner_size;
	int *sh_corner_UL_in; // Upper Left
	int *sh_corner_UL_out;
	int *sh_corner_UR_in; // Upper Right
	int *sh_corner_UR_out;
	int *sh_corner_DL_in; // Down Left
	int *sh_corner_DL_out;
	int *sh_corner_DR_in; // Down Right
	int *sh_corner_DR_out;

	int *output_send_tab;

	void share_horizontal();
	void share_vertical();
	void share_depth();
	void share_corners();

	void share();

	void pre_share_copy();
	void post_share_copy();

	void iter();

	void send_to_master();
	
public:

	Node(int rank, int grid_size, int node_size);
	~Node();

	void main();

};

#endif