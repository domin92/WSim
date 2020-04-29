#ifndef NODE_HPP
#define NODE_HPP

class Node{

	int rank;
	int grid_size; // Number of nodes in side of 3d grid
	int node_size; // Size of the cude side in bytes
    int node_volume;

    int share_thickness; // Thickness of shared areas in bytes

    int number_of_main_arrays;

	int main_array_size;
	char ***array[2]; // Two 3D arrays - input, output
	int current_array_idx; // Index of the current input array

	// Position in grid
	int x_pos_in_grid;
	int y_pos_in_grid;
	int z_pos_in_grid;

	// Share arrays
	int sh_horizontal_size;
    char* sh_horizontal_L_in; // Left
	char *sh_horizontal_L_out;
	char *sh_horizontal_R_in; // Right
	char *sh_horizontal_R_out;

	int sh_vertical_size;
	char *sh_vertical_U_in; // Up
	char *sh_vertical_U_out;
	char *sh_vertical_D_in; // Down
	char *sh_vertical_D_out;

	int sh_depth_size;
	char *sh_depth_F_in; // Front
	char *sh_depth_F_out;
	char *sh_depth_B_in; // Back
	char *sh_depth_B_out;

	int sh_corner_size;
	char *sh_corner_FUL_in; // Front Upper Left
	char *sh_corner_FUL_out;
	char *sh_corner_FUR_in; // Front Upper Right
	char *sh_corner_FUR_out;
	char *sh_corner_FDL_in; // Front Down Left
	char *sh_corner_FDL_out;
	char *sh_corner_FDR_in; // Front Down Right
	char *sh_corner_FDR_out;
    char *sh_corner_BUL_in; // Back Upper Left
	char *sh_corner_BUL_out;
	char *sh_corner_BUR_in; // Back Upper Right
	char *sh_corner_BUR_out;
	char *sh_corner_BDL_in; // Back Down Left
	char *sh_corner_BDL_out;
	char *sh_corner_BDR_in; // Back Down Right
	char *sh_corner_BDR_out;

    int sh_edge_size;
    char *sh_edge_UL_in; // Up Left
    char *sh_edge_UL_out;
    char *sh_edge_UR_in; // Up Right
    char *sh_edge_UR_out;
    char *sh_edge_DL_in; // Down Left
    char *sh_edge_DL_out;
    char *sh_edge_DR_in; // Down Right
    char *sh_edge_DR_out;
    char *sh_edge_FL_in; // Front Left
    char *sh_edge_FL_out;
    char *sh_edge_FR_in; // Front Right
    char *sh_edge_FR_out;
    char *sh_edge_FU_in; // Front Up
    char *sh_edge_FU_out;
    char *sh_edge_FD_in; // Front Down
    char *sh_edge_FD_out;
    char *sh_edge_BL_in; // Back Left
    char *sh_edge_BL_out;
    char *sh_edge_BR_in; // Back Right
    char *sh_edge_BR_out;
    char *sh_edge_BU_in; // Back Up
    char *sh_edge_BU_out;
    char *sh_edge_BD_in; // Back Down
    char *sh_edge_BD_out;

	char *send_array;

    bool node_in_grid(int x, int y, int z);
    int rank_with_offset(int x, int y, int z);
    void recv_buffer(bool condition, char *intput_buffer, char *output_buffer, int size, int in_x, int in_y, int in_z);

	void share_horizontal();
	void share_vertical();
	void share_depth();
	void share_corners();
	void share_edges();

	void share();

    void pre_share_copy_buffer(char *output_buffer, int size_x, int size_y, int size_z, int out_x, int out_y, int out_z);
    void post_share_copy_buffer(char *input_buffer, int size_x, int size_y, int size_z, int out_x, int out_y, int out_z);

	void pre_share_copy();
	void post_share_copy();

	void iter();

	void receive_from_master();
	void send_to_master();
	
public:

	Node(int rank, int grid_size, int node_size);
	~Node();

	void main();

};

#endif