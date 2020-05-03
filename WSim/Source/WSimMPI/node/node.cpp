#include "Node.hpp"

#include <cstdlib>
#include <iostream>
#include <mpi.h>

ShareBuffers::ShareBuffers(int sh_horizontal_size, int sh_vertical_size, int sh_depth_size, int sh_corner_size, int sh_edge_size) {
    sh_horizontal_L_in = new char[sh_horizontal_size];
    sh_horizontal_L_out = new char[sh_horizontal_size];
    sh_horizontal_R_in = new char[sh_horizontal_size];
    sh_horizontal_R_out = new char[sh_horizontal_size];

    sh_vertical_U_in = new char[sh_vertical_size];
    sh_vertical_U_out = new char[sh_vertical_size];
    sh_vertical_D_in = new char[sh_vertical_size];
    sh_vertical_D_out = new char[sh_vertical_size];

    sh_depth_F_in = new char[sh_depth_size];
    sh_depth_F_out = new char[sh_depth_size];
    sh_depth_B_in = new char[sh_depth_size];
    sh_depth_B_out = new char[sh_depth_size];

    sh_corner_FUL_in = new char[sh_corner_size];
    sh_corner_FUL_out = new char[sh_corner_size];
    sh_corner_FUR_in = new char[sh_corner_size];
    sh_corner_FUR_out = new char[sh_corner_size];
    sh_corner_FDL_in = new char[sh_corner_size];
    sh_corner_FDL_out = new char[sh_corner_size];
    sh_corner_FDR_in = new char[sh_corner_size];
    sh_corner_FDR_out = new char[sh_corner_size];
    sh_corner_BUL_in = new char[sh_corner_size];
    sh_corner_BUL_out = new char[sh_corner_size];
    sh_corner_BUR_in = new char[sh_corner_size];
    sh_corner_BUR_out = new char[sh_corner_size];
    sh_corner_BDL_in = new char[sh_corner_size];
    sh_corner_BDL_out = new char[sh_corner_size];
    sh_corner_BDR_in = new char[sh_corner_size];
    sh_corner_BDR_out = new char[sh_corner_size];

    sh_edge_UL_in = new char[sh_edge_size];
    sh_edge_UL_out = new char[sh_edge_size];
    sh_edge_UR_in = new char[sh_edge_size];
    sh_edge_UR_out = new char[sh_edge_size];
    sh_edge_DL_in = new char[sh_edge_size];
    sh_edge_DL_out = new char[sh_edge_size];
    sh_edge_DR_in = new char[sh_edge_size];
    sh_edge_DR_out = new char[sh_edge_size];
    sh_edge_FL_in = new char[sh_edge_size];
    sh_edge_FL_out = new char[sh_edge_size];
    sh_edge_FR_in = new char[sh_edge_size];
    sh_edge_FR_out = new char[sh_edge_size];
    sh_edge_FU_in = new char[sh_edge_size];
    sh_edge_FU_out = new char[sh_edge_size];
    sh_edge_FD_in = new char[sh_edge_size];
    sh_edge_FD_out = new char[sh_edge_size];
    sh_edge_BL_in = new char[sh_edge_size];
    sh_edge_BL_out = new char[sh_edge_size];
    sh_edge_BR_in = new char[sh_edge_size];
    sh_edge_BR_out = new char[sh_edge_size];
    sh_edge_BU_in = new char[sh_edge_size];
    sh_edge_BU_out = new char[sh_edge_size];
    sh_edge_BD_in = new char[sh_edge_size];
    sh_edge_BD_out = new char[sh_edge_size];
}

Node::Node(int rank, int grid_size, int node_size)
    : share_thickness(1),
      number_of_main_arrays(2),
      sh_horizontal_size(node_size* node_size* share_thickness* number_of_main_arrays),
      sh_vertical_size(node_size* node_size* share_thickness* number_of_main_arrays),
      sh_depth_size(node_size* node_size* share_thickness* number_of_main_arrays),
      sh_corner_size(share_thickness* share_thickness* share_thickness* number_of_main_arrays),
      sh_edge_size(share_thickness* share_thickness* node_size* number_of_main_arrays),
      shareBuffers(sh_horizontal_size, sh_vertical_size, sh_depth_size, sh_corner_size, sh_edge_size) {
    this->rank = rank;
    this->grid_size = grid_size;
    this->node_size = node_size;
    node_volume = node_size * node_size * node_size;

    // Calculating node position in 3D space - VERY IMPORTANT!
    int adjusted_rank = rank - 1; // Rank excluding master
    x_pos_in_grid = (adjusted_rank) % grid_size;
    y_pos_in_grid = ((adjusted_rank) % (grid_size * grid_size)) / grid_size;
    z_pos_in_grid = (adjusted_rank) / (grid_size * grid_size);
    // For grid_size = 2 we have:
    //
    // Rank | x | y | z
    // 1    | 0 | 0 | 0
    // 2    | 1 | 0 | 0
    // 3    | 0 | 1 | 0
    // 4    | 1 | 1 | 0
    // 5    | 0 | 0 | 1
    // 6    | 1 | 0 | 1
    // 7    | 0 | 1 | 1
    // 8    | 1 | 1 | 1
    //

    // Creating two 3D arrays
    main_array_size = node_size + 2 * share_thickness;

    array[0] = new char **[main_array_size * number_of_main_arrays];
    for (int i = 0; i < main_array_size * number_of_main_arrays; i++) {

        array[0][i] = new char *[main_array_size];
        for (int j = 0; j < main_array_size; j++) {

            array[0][i][j] = new char[main_array_size];
            for (int k = 0; k < main_array_size; k++) {

                array[0][i][j][k] = 0;
            }
        }
    }

    array[1] = new char **[main_array_size * number_of_main_arrays];
    for (int i = 0; i < main_array_size * number_of_main_arrays; i++) {

        array[1][i] = new char *[main_array_size];
        for (int j = 0; j < main_array_size; j++) {

            array[1][i][j] = new char[main_array_size];
            for (int k = 0; k < main_array_size; k++) {

                array[1][i][j][k] = 0;
            }
        }
    }

    current_array_idx = 0;

    send_array = new char[node_volume];
}

ShareBuffers::~ShareBuffers() {
    delete[] sh_horizontal_L_in;
    delete[] sh_horizontal_L_out;
    delete[] sh_horizontal_R_in;
    delete[] sh_horizontal_R_out;
    delete[] sh_vertical_U_in;
    delete[] sh_vertical_U_out;
    delete[] sh_vertical_D_in;
    delete[] sh_vertical_D_out;
    delete[] sh_depth_F_in;
    delete[] sh_depth_F_out;
    delete[] sh_depth_B_in;
    delete[] sh_depth_B_out;
    delete[] sh_corner_FUL_in;
    delete[] sh_corner_FUL_out;
    delete[] sh_corner_FUR_in;
    delete[] sh_corner_FUR_out;
    delete[] sh_corner_FDL_in;
    delete[] sh_corner_FDL_out;
    delete[] sh_corner_FDR_in;
    delete[] sh_corner_FDR_out;
    delete[] sh_corner_BUL_in;
    delete[] sh_corner_BUL_out;
    delete[] sh_corner_BUR_in;
    delete[] sh_corner_BUR_out;
    delete[] sh_corner_BDL_in;
    delete[] sh_corner_BDL_out;
    delete[] sh_corner_BDR_in;
    delete[] sh_corner_BDR_out;
    delete[] sh_edge_UL_in;
    delete[] sh_edge_UL_out;
    delete[] sh_edge_UR_in;
    delete[] sh_edge_UR_out;
    delete[] sh_edge_DL_in;
    delete[] sh_edge_DL_out;
    delete[] sh_edge_DR_in;
    delete[] sh_edge_DR_out;
    delete[] sh_edge_FL_in;
    delete[] sh_edge_FL_out;
    delete[] sh_edge_FR_in;
    delete[] sh_edge_FR_out;
    delete[] sh_edge_FU_in;
    delete[] sh_edge_FU_out;
    delete[] sh_edge_FD_in;
    delete[] sh_edge_FD_out;
    delete[] sh_edge_BL_in;
    delete[] sh_edge_BL_out;
    delete[] sh_edge_BR_in;
    delete[] sh_edge_BR_out;
    delete[] sh_edge_BU_in;
    delete[] sh_edge_BU_out;
    delete[] sh_edge_BD_in;
    delete[] sh_edge_BD_out;
}

Node::~Node() {
    delete[] send_array;

    for (int i = 0; i < main_array_size *number_of_main_arrays; i++) {
        for (int j = 0; j < main_array_size; j++) {
            delete[] array[0][i][j];
            delete[] array[1][i][j];
        }
        delete[] array[0][i];
        delete[] array[1][i];
    }

    delete[] array[0];
    delete[] array[1];
}

inline bool Node::node_in_grid(int x, int y, int z) {
    // Returns true if neighbour node is withing the grid
    bool x_in_grid = (x_pos_in_grid + x >= 0) & (x_pos_in_grid + x < grid_size);
    bool y_in_grid = (y_pos_in_grid + y >= 0) & (y_pos_in_grid + y < grid_size);
    bool z_in_grid = (z_pos_in_grid + z >= 0) & (z_pos_in_grid + z < grid_size);
    return x_in_grid & y_in_grid & z_in_grid;
}

inline int Node::rank_with_offset(int x, int y, int z) {
    // Returns neighbour node rank
    return rank + z * grid_size * grid_size + y * grid_size + x;
}

inline void Node::recv_buffer(bool condition, char *intput_buffer, char *output_buffer, int size, int in_x, int in_y, int in_z) {
    if (condition) {
        if (node_in_grid(in_x, in_y, in_z)) {
            MPI_Recv(intput_buffer, size, MPI_CHAR, rank_with_offset(in_x, in_y, in_z), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (node_in_grid(-in_x, -in_y, -in_z)) {
            MPI_Send(output_buffer, size, MPI_CHAR, rank_with_offset(-in_x, -in_y, -in_z), 1, MPI_COMM_WORLD);
        }
    }
}

void Node::share_vertical() {
    recv_buffer(y_pos_in_grid % 2 == 0, shareBuffers.sh_vertical_U_in, shareBuffers.sh_vertical_D_out, sh_vertical_size, 0, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 0, shareBuffers.sh_vertical_D_in, shareBuffers.sh_vertical_U_out, sh_vertical_size, 0, 1, 0);

    recv_buffer(y_pos_in_grid % 2 == 1, shareBuffers.sh_vertical_U_in, shareBuffers.sh_vertical_D_out, sh_vertical_size, 0, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 1, shareBuffers.sh_vertical_D_in, shareBuffers.sh_vertical_U_out, sh_vertical_size, 0, 1, 0);
}

void Node::share_horizontal() {
    recv_buffer(x_pos_in_grid % 2 == 0, shareBuffers.sh_horizontal_L_in, shareBuffers.sh_horizontal_R_out, sh_horizontal_size, -1, 0, 0);
    recv_buffer(x_pos_in_grid % 2 == 0, shareBuffers.sh_horizontal_R_in, shareBuffers.sh_horizontal_L_out, sh_horizontal_size, 1, 0, 0);

    recv_buffer(x_pos_in_grid % 2 == 1, shareBuffers.sh_horizontal_L_in, shareBuffers.sh_horizontal_R_out, sh_horizontal_size, -1, 0, 0);
    recv_buffer(x_pos_in_grid % 2 == 1, shareBuffers.sh_horizontal_R_in, shareBuffers.sh_horizontal_L_out, sh_horizontal_size, 1, 0, 0);
}

void Node::share_depth() {
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_depth_F_in, shareBuffers.sh_depth_B_out, sh_depth_size, 0, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_depth_B_in, shareBuffers.sh_depth_F_out, sh_depth_size, 0, 0, 1);

    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_depth_F_in, shareBuffers.sh_depth_B_out, sh_depth_size, 0, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_depth_B_in, shareBuffers.sh_depth_F_out, sh_depth_size, 0, 0, 1);
}

void Node::share_corners() {
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_corner_FUL_in, shareBuffers.sh_corner_BDR_out, sh_corner_size, -1, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_corner_BDR_in, shareBuffers.sh_corner_FUL_out, sh_corner_size, 1, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_corner_FUR_in, shareBuffers.sh_corner_BDL_out, sh_corner_size, 1, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_corner_BDL_in, shareBuffers.sh_corner_FUR_out, sh_corner_size, -1, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_corner_FDL_in, shareBuffers.sh_corner_BUR_out, sh_corner_size, -1, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_corner_BUR_in, shareBuffers.sh_corner_FDL_out, sh_corner_size, 1, -1, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_corner_FDR_in, shareBuffers.sh_corner_BUL_out, sh_corner_size, 1, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_corner_BUL_in, shareBuffers.sh_corner_FDR_out, sh_corner_size, -1, -1, 1);

    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_corner_FUL_in, shareBuffers.sh_corner_BDR_out, sh_corner_size, -1, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_corner_BDR_in, shareBuffers.sh_corner_FUL_out, sh_corner_size, 1, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_corner_FUR_in, shareBuffers.sh_corner_BDL_out, sh_corner_size, 1, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_corner_BDL_in, shareBuffers.sh_corner_FUR_out, sh_corner_size, -1, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_corner_FDL_in, shareBuffers.sh_corner_BUR_out, sh_corner_size, -1, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_corner_BUR_in, shareBuffers.sh_corner_FDL_out, sh_corner_size, 1, -1, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_corner_FDR_in, shareBuffers.sh_corner_BUL_out, sh_corner_size, 1, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_corner_BUL_in, shareBuffers.sh_corner_FDR_out, sh_corner_size, -1, -1, 1);
}

void Node::share_edges() {
    recv_buffer(y_pos_in_grid % 2 == 0, shareBuffers.sh_edge_UL_in, shareBuffers.sh_edge_DR_out,sh_edge_size, -1, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 0, shareBuffers.sh_edge_DR_in, shareBuffers.sh_edge_UL_out,sh_edge_size, 1, 1, 0);
    recv_buffer(y_pos_in_grid % 2 == 0, shareBuffers.sh_edge_UR_in, shareBuffers.sh_edge_DL_out,sh_edge_size, 1, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 0, shareBuffers.sh_edge_DL_in, shareBuffers.sh_edge_UR_out,sh_edge_size, -1, 1, 0);

    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_edge_FL_in, shareBuffers.sh_edge_BR_out,sh_edge_size, -1, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_edge_BR_in, shareBuffers.sh_edge_FL_out,sh_edge_size, 1, 0, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_edge_FR_in, shareBuffers.sh_edge_BL_out,sh_edge_size, 1, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_edge_BL_in, shareBuffers.sh_edge_FR_out,sh_edge_size, -1, 0, 1);

    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_edge_FU_in, shareBuffers.sh_edge_BD_out,sh_edge_size, 0, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_edge_BD_in, shareBuffers.sh_edge_FU_out,sh_edge_size, 0, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_edge_FD_in, shareBuffers.sh_edge_BU_out,sh_edge_size, 0, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, shareBuffers.sh_edge_BU_in, shareBuffers.sh_edge_FD_out,sh_edge_size, 0, -1, 1);

    recv_buffer(y_pos_in_grid % 2 == 1, shareBuffers.sh_edge_UL_in, shareBuffers.sh_edge_DR_out,sh_edge_size, -1, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 1, shareBuffers.sh_edge_DR_in, shareBuffers.sh_edge_UL_out,sh_edge_size, 1, 1, 0);
    recv_buffer(y_pos_in_grid % 2 == 1, shareBuffers.sh_edge_UR_in, shareBuffers.sh_edge_DL_out,sh_edge_size, 1, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 1, shareBuffers.sh_edge_DL_in, shareBuffers.sh_edge_UR_out,sh_edge_size, -1, 1, 0);

    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_edge_FL_in, shareBuffers.sh_edge_BR_out,sh_edge_size, -1, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_edge_BR_in, shareBuffers.sh_edge_FL_out,sh_edge_size, 1, 0, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_edge_FR_in, shareBuffers.sh_edge_BL_out,sh_edge_size, 1, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_edge_BL_in, shareBuffers.sh_edge_FR_out,sh_edge_size, -1, 0, 1);

    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_edge_FU_in, shareBuffers.sh_edge_BD_out,sh_edge_size, 0, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_edge_BD_in, shareBuffers.sh_edge_FU_out,sh_edge_size, 0, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_edge_FD_in, shareBuffers.sh_edge_BU_out,sh_edge_size, 0, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, shareBuffers.sh_edge_BU_in, shareBuffers.sh_edge_FD_out,sh_edge_size, 0, -1, 1);
}

inline void Node::pre_share_copy_buffer(char* output_buffer, int size_x, int size_y, int size_z, int out_x, int out_y, int out_z) {
    if (node_in_grid(out_x, out_y, out_z)) {
        for (int z = 0; z < size_z; z++) {
            for (int y = 0; y < size_y; y++) {
                for (int x = 0; x < size_x; x++) {
                    int x_offset = (out_x == 1 ? node_size : share_thickness);
                    int y_offset = (out_y == 1 ? node_size : share_thickness);
                    int z_offset = (out_z == 1 ? node_size : share_thickness);
                    output_buffer[z * size_y * size_x + y * size_x + x] = array[current_array_idx][z_offset + z][y_offset + y][x_offset + x];
                }
            }
        }
    }
}

void Node::pre_share_copy() {
    pre_share_copy_buffer(shareBuffers.sh_vertical_D_out, node_size, share_thickness, node_size, 0, 1, 0);
    pre_share_copy_buffer(shareBuffers.sh_vertical_U_out, node_size, share_thickness, node_size, 0, -1, 0);
    pre_share_copy_buffer(shareBuffers.sh_horizontal_R_out, share_thickness, node_size, node_size, 1, 0, 0);
    pre_share_copy_buffer(shareBuffers.sh_horizontal_L_out, share_thickness, node_size, node_size, -1, 0, 0);
    pre_share_copy_buffer(shareBuffers.sh_depth_B_out, node_size, node_size, share_thickness, 0, 0, 1);
    pre_share_copy_buffer(shareBuffers.sh_depth_F_out, node_size, node_size, share_thickness, 0, 0, -1);

    pre_share_copy_buffer(shareBuffers.sh_corner_FUL_out, share_thickness, share_thickness, share_thickness, -1, -1, -1);
    pre_share_copy_buffer(shareBuffers.sh_corner_BDR_out, share_thickness, share_thickness, share_thickness, 1, 1, 1);
    pre_share_copy_buffer(shareBuffers.sh_corner_FUR_out, share_thickness, share_thickness, share_thickness, 1, -1, -1);
    pre_share_copy_buffer(shareBuffers.sh_corner_BDL_out, share_thickness, share_thickness, share_thickness, -1, 1, 1);
    pre_share_copy_buffer(shareBuffers.sh_corner_FDL_out, share_thickness, share_thickness, share_thickness, -1, 1, -1);
    pre_share_copy_buffer(shareBuffers.sh_corner_BUR_out, share_thickness, share_thickness, share_thickness, 1, -1, 1);
    pre_share_copy_buffer(shareBuffers.sh_corner_FDR_out, share_thickness, share_thickness, share_thickness, 1, 1, -1);
    pre_share_copy_buffer(shareBuffers.sh_corner_BUL_out, share_thickness, share_thickness, share_thickness, -1, -1, 1);

    pre_share_copy_buffer(shareBuffers.sh_edge_UL_out, share_thickness, share_thickness, node_size, -1, -1, 0);
    pre_share_copy_buffer(shareBuffers.sh_edge_DR_out, share_thickness, share_thickness, node_size, 1, 1, 0);
    pre_share_copy_buffer(shareBuffers.sh_edge_UR_out, share_thickness, share_thickness, node_size, 1, -1, 0);
    pre_share_copy_buffer(shareBuffers.sh_edge_DL_out, share_thickness, share_thickness, node_size, -1, 1, 0);

    pre_share_copy_buffer(shareBuffers.sh_edge_FL_out, share_thickness, node_size, share_thickness, -1, 0, -1);
    pre_share_copy_buffer(shareBuffers.sh_edge_BR_out, share_thickness, node_size, share_thickness, 1, 0, 1);
    pre_share_copy_buffer(shareBuffers.sh_edge_FR_out, share_thickness, node_size, share_thickness, 1, 0, -1);
    pre_share_copy_buffer(shareBuffers.sh_edge_BL_out, share_thickness, node_size, share_thickness, -1, 0, 1);

    pre_share_copy_buffer(shareBuffers.sh_edge_FU_out, node_size, share_thickness, share_thickness, 0, -1, -1);
    pre_share_copy_buffer(shareBuffers.sh_edge_BD_out, node_size, share_thickness, share_thickness, 0, 1, 1);
    pre_share_copy_buffer(shareBuffers.sh_edge_FD_out, node_size, share_thickness, share_thickness, 0, 1, -1);
    pre_share_copy_buffer(shareBuffers.sh_edge_BU_out, node_size, share_thickness, share_thickness, 0, -1, 1);
}

inline void Node::post_share_copy_buffer(char *input_buffer, int size_x, int size_y, int size_z, int out_x, int out_y, int out_z) {
    if (node_in_grid(out_x, out_y, out_z)) {
        for (int z = 0; z < size_z; z++) {
            for (int y = 0; y < size_y; y++) {
                for (int x = 0; x < size_x; x++) {
                    int x_offset = (out_x == 1 ? node_size + share_thickness : (out_x == 0 ? share_thickness : 0));
                    int y_offset = (out_y == 1 ? node_size + share_thickness : (out_y == 0 ? share_thickness : 0));
                    int z_offset = (out_z == 1 ? node_size + share_thickness : (out_z == 0 ? share_thickness : 0));
                    array[current_array_idx][z_offset + z][y_offset + y][x_offset + x] = input_buffer[z * size_y * size_x + y * size_x + x];
                }
            }
        }
    }
}

void Node::post_share_copy() {
    post_share_copy_buffer(shareBuffers.sh_vertical_U_in, node_size, share_thickness, node_size, 0, -1, 0);
    post_share_copy_buffer(shareBuffers.sh_vertical_D_in, node_size, share_thickness, node_size, 0, 1, 0);
    post_share_copy_buffer(shareBuffers.sh_horizontal_L_in, share_thickness, node_size, node_size, -1, 0, 0);
    post_share_copy_buffer(shareBuffers.sh_horizontal_R_in, share_thickness, node_size, node_size, 1, 0, 0);
    post_share_copy_buffer(shareBuffers.sh_depth_B_in, node_size, node_size, share_thickness, 0, 0, 1);
    post_share_copy_buffer(shareBuffers.sh_depth_F_in, node_size, node_size, share_thickness, 0, 0, -1);

    post_share_copy_buffer(shareBuffers.sh_corner_FUL_in, share_thickness, share_thickness, share_thickness, -1, -1, -1);
    post_share_copy_buffer(shareBuffers.sh_corner_BDR_in, share_thickness, share_thickness, share_thickness, 1, 1, 1);
    post_share_copy_buffer(shareBuffers.sh_corner_FUR_in, share_thickness, share_thickness, share_thickness, 1, -1, -1);
    post_share_copy_buffer(shareBuffers.sh_corner_BDL_in, share_thickness, share_thickness, share_thickness, -1, 1, 1);
    post_share_copy_buffer(shareBuffers.sh_corner_FDL_in, share_thickness, share_thickness, share_thickness, -1, 1, -1);
    post_share_copy_buffer(shareBuffers.sh_corner_BUR_in, share_thickness, share_thickness, share_thickness, 1, -1, 1);
    post_share_copy_buffer(shareBuffers.sh_corner_FDR_in, share_thickness, share_thickness, share_thickness, 1, 1, -1);
    post_share_copy_buffer(shareBuffers.sh_corner_BUL_in, share_thickness, share_thickness, share_thickness, -1, -1, 1);

    post_share_copy_buffer(shareBuffers.sh_edge_UL_in, share_thickness, share_thickness, node_size, -1, -1, 0);
    post_share_copy_buffer(shareBuffers.sh_edge_DR_in, share_thickness, share_thickness, node_size, 1, 1, 0);
    post_share_copy_buffer(shareBuffers.sh_edge_UR_in, share_thickness, share_thickness, node_size, 1, -1, 0);
    post_share_copy_buffer(shareBuffers.sh_edge_DL_in, share_thickness, share_thickness, node_size, -1, 1, 0);

    post_share_copy_buffer(shareBuffers.sh_edge_FL_in, share_thickness, node_size, share_thickness, -1, 0, -1);
    post_share_copy_buffer(shareBuffers.sh_edge_BR_in, share_thickness, node_size, share_thickness, 1, 0, 1);
    post_share_copy_buffer(shareBuffers.sh_edge_FR_in, share_thickness, node_size, share_thickness, 1, 0, -1);
    post_share_copy_buffer(shareBuffers.sh_edge_BL_in, share_thickness, node_size, share_thickness, -1, 0, 1);

    post_share_copy_buffer(shareBuffers.sh_edge_FU_in, node_size, share_thickness, share_thickness, 0, -1, -1);
    post_share_copy_buffer(shareBuffers.sh_edge_BD_in, node_size, share_thickness, share_thickness, 0, 1, 1);
    post_share_copy_buffer(shareBuffers.sh_edge_FD_in, node_size, share_thickness, share_thickness, 0, 1, -1);
    post_share_copy_buffer(shareBuffers.sh_edge_BU_in, node_size, share_thickness, share_thickness, 0, -1, 1);
}

void Node::iter() {

    char ***input_array = array[current_array_idx];
    char ***output_array = array[current_array_idx ^ 1];

    for (int z = share_thickness; z < node_size + share_thickness; z++) {
        for (int y = share_thickness; y < node_size + share_thickness; y++) {
            for (int x = share_thickness; x < node_size + share_thickness; x++) {

                input_array[main_array_size + z][y][x] = 69;
                output_array[main_array_size + z][y][x] = 69;

                int val = 0;

                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        for (int k = -1; k <= 1; k++) {
                            if (i == 0 && j == 0 && z == 0) {
                                continue;
                            }

                            val += input_array[z + i][y + j][x + k] == 1;
                        }
                    }
                }

                if (input_array[z][y][x] == 0) {
                    if ((val >= 13 && val <= 14) || val == 17 || val == 18 || val == 19) {
                        output_array[z][y][x] = 1;
                    } else {
                        output_array[z][y][x] = 0;
                    }
                } else {
                    if (val >= 13 && val <= 26) {
                        output_array[z][y][x] = 1;
                    } else {
                        output_array[z][y][x] = 0;
                    }
                }
            }
        }
    }

    current_array_idx ^= 1;
}

void Node::receive_from_master() {
    MPI_Scatter(MPI_IN_PLACE, 0, MPI_CHAR, send_array, node_volume, MPI_CHAR, 0, MPI_COMM_WORLD);

    for (int z = 0; z < node_size; z++) {
        for (int y = 0; y < node_size; y++) {
            for (int x = 0; x < node_size; x++) {
                array[current_array_idx][z + share_thickness][y + share_thickness][x + share_thickness] = send_array[z * (node_size * node_size) + y * node_size + x];
            }
        }
    }
}

void Node::send_to_master() {
    for (int z = 0; z < node_size; z++) {
        for (int y = 0; y < node_size; y++) {
            for (int x = 0; x < node_size; x++) {
                send_array[z * (node_size * node_size) + y * node_size + x] = array[current_array_idx][z + share_thickness][y + share_thickness][x + share_thickness];
            }
        }
    }

    MPI_Gather(send_array, node_volume, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Node::share() {
    share_vertical();
    share_horizontal();
    share_depth();
    share_corners();
    share_edges();
}

void Node::main() {
    receive_from_master();
    while (true) {
        pre_share_copy();
        share();
        post_share_copy();
        iter();
        send_to_master();
    }
}
