#ifndef NODE_HPP
#define NODE_HPP

class Node{

	int rank;
	int grid_size; // Number of nodes in side of 3d grd
	int node_size; // Size of the cude side in bytes

	int share_thickness; // Thickness of shared areas in bytes

	char **tab[2]; // Two 2D arrays - input, output
	int current_tab_idx; // Index of the current input tab

	// Position in grid
	int row;
	int col;

	// Share arrays
	int sh_horizontal_size;
	char *sh_horizontal_L_in; // Left
	char *sh_horizontal_L_out;
	char *sh_horizontal_R_in; // Right
	char *sh_horizontal_R_out;

	int sh_vertical_size;
	char *sh_vertical_U_in; // Up
	char *sh_vertical_U_out;
	char *sh_vertical_D_in; // Down
	char *sh_vertical_D_out;

	int sh_corner_size;
	char *sh_corner_UL_in; // Upper Left
	char *sh_corner_UL_out;
	char *sh_corner_UR_in; // Upper Right
	char *sh_corner_UR_out;
	char *sh_corner_DL_in; // Down Left
	char *sh_corner_DL_out;
	char *sh_corner_DR_in; // Down Right
	char *sh_corner_DR_out;

	char *output_send_tab;

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