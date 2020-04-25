#include "Node.hpp"

#include <cstdlib>
#include <iostream>
#include <mpi.h>

Node::Node(int rank, int grid_size, int node_size) {
    this->rank = rank;
    this->grid_size = grid_size;
    this->node_size = node_size;

    this->share_thickness = 1;

    sh_horizontal_size = node_size * node_size * share_thickness;
    sh_horizontal_L_in = new char[sh_horizontal_size];
    sh_horizontal_L_out = new char[sh_horizontal_size];
    sh_horizontal_R_in = new char[sh_horizontal_size];
    sh_horizontal_R_out = new char[sh_horizontal_size];

    sh_vertical_size = node_size * node_size * share_thickness;
    sh_vertical_U_in = new char[sh_vertical_size];
    sh_vertical_U_out = new char[sh_vertical_size];
    sh_vertical_D_in = new char[sh_vertical_size];
    sh_vertical_D_out = new char[sh_vertical_size];

    sh_depth_size = node_size * node_size * share_thickness;
    sh_depth_F_in = new char[sh_depth_size];
    sh_depth_F_out = new char[sh_depth_size];
    sh_depth_B_in = new char[sh_depth_size];
    sh_depth_B_out = new char[sh_depth_size];

    sh_corner_size = share_thickness * share_thickness * share_thickness;
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

    sh_edge_size = share_thickness * share_thickness * node_size;
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

    adjusted_rank = rank - 1;

    // Calculating node position in 3D space - VERY IMPORTANT!
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

    array[0] = new char **[main_array_size];
    for (int i = 0; i < main_array_size; i++) {

        array[0][i] = new char *[main_array_size];
        for (int j = 0; j < main_array_size; j++) {

            array[0][i][j] = new char[main_array_size];
            for (int k = 0; k < main_array_size; k++) {

                array[0][i][j][k] = 0;
            }
        }
    }

    array[1] = new char **[main_array_size];
    for (int i = 0; i < main_array_size; i++) {

        array[1][i] = new char *[main_array_size];
        for (int j = 0; j < main_array_size; j++) {

            array[1][i][j] = new char[main_array_size];
            for (int k = 0; k < main_array_size; k++) {

                array[1][i][j][k] = 0;
            }
        }
    }

    current_array_idx = 0;

    send_array = new char[node_size * node_size * node_size];
}

Node::~Node() {

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

    delete[] send_array;

    for (int i = 0; i < main_array_size; i++) {
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

bool Node::node_in_grid(int x, int y, int z) {
    // Returns true if neighbour node is withing grid
    bool x_in_grid = x_pos_in_grid + x >= 0 & x_pos_in_grid + x < grid_size;
    bool y_in_grid = y_pos_in_grid + y >= 0 & y_pos_in_grid + y < grid_size;
    bool z_in_grid = z_pos_in_grid + z >= 0 & z_pos_in_grid + z < grid_size;
    return x_in_grid & y_in_grid & z_in_grid;
}

int Node::rank_with_offset(int x, int y, int z) {
    return rank + z * grid_size * grid_size + y * grid_size + x;
}

void Node::share_vertical() {

    if (y_pos_in_grid % 2 == 0) {
        if (y_pos_in_grid - 1 >= 0) {
            MPI_Recv(sh_vertical_U_in, sh_vertical_size, MPI_CHAR, rank - grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (y_pos_in_grid + 1 < grid_size) {
            MPI_Send(sh_vertical_D_out, sh_vertical_size, MPI_CHAR, rank + grid_size, 1, MPI_COMM_WORLD);
        }
    }

    if (y_pos_in_grid % 2 == 0) {
        if (y_pos_in_grid + 1 < grid_size) {
            MPI_Recv(sh_vertical_D_in, sh_vertical_size, MPI_CHAR, rank + grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (y_pos_in_grid - 1 >= 0) {
            MPI_Send(sh_vertical_U_out, sh_vertical_size, MPI_CHAR, rank - grid_size, 1, MPI_COMM_WORLD);
        }
    }

    if (y_pos_in_grid % 2 == 1) {
        if (y_pos_in_grid - 1 >= 0) {
            MPI_Recv(sh_vertical_U_in, sh_vertical_size, MPI_CHAR, rank - grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (y_pos_in_grid + 1 < grid_size) {
            MPI_Send(sh_vertical_D_out, sh_vertical_size, MPI_CHAR, rank + grid_size, 1, MPI_COMM_WORLD);
        }
    }

    if (y_pos_in_grid % 2 == 1) {
        if (y_pos_in_grid + 1 < grid_size) {
            MPI_Recv(sh_vertical_D_in, sh_vertical_size, MPI_CHAR, rank + grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (y_pos_in_grid - 1 >= 0) {
            MPI_Send(sh_vertical_U_out, sh_vertical_size, MPI_CHAR, rank - grid_size, 1, MPI_COMM_WORLD);
        }
    }
}

void Node::share_horizontal() {

    if (x_pos_in_grid % 2 == 0) {
        if (x_pos_in_grid - 1 >= 0) {
            MPI_Recv(sh_horizontal_L_in, sh_horizontal_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (x_pos_in_grid + 1 < grid_size) {
            MPI_Send(sh_horizontal_R_out, sh_horizontal_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD);
        }
    }

    if (x_pos_in_grid % 2 == 0) {
        if (x_pos_in_grid + 1 < grid_size) {
            MPI_Recv(sh_horizontal_R_in, sh_horizontal_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (x_pos_in_grid - 1 >= 0) {
            MPI_Send(sh_horizontal_L_out, sh_horizontal_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD);
        }
    }

    if (x_pos_in_grid % 2 == 1) {
        if (x_pos_in_grid - 1 >= 0) {
            MPI_Recv(sh_horizontal_L_in, sh_horizontal_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (x_pos_in_grid + 1 < grid_size) {
            MPI_Send(sh_horizontal_R_out, sh_horizontal_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD);
        }
    }

    if (x_pos_in_grid % 2 == 1) {
        if (x_pos_in_grid + 1 < grid_size) {
            MPI_Recv(sh_horizontal_R_in, sh_horizontal_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (x_pos_in_grid - 1 >= 0) {
            MPI_Send(sh_horizontal_L_out, sh_horizontal_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD);
        }
    }
}

void Node::share_depth() {

    if (z_pos_in_grid % 2 == 0) {
        if (z_pos_in_grid - 1 >= 0) {
            MPI_Recv(sh_depth_F_in, sh_depth_size, MPI_CHAR, rank - grid_size * grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (z_pos_in_grid + 1 < grid_size) {
            MPI_Send(sh_depth_B_out, sh_depth_size, MPI_CHAR, rank + grid_size * grid_size, 1, MPI_COMM_WORLD);
        }
    }

    if (z_pos_in_grid % 2 == 0) {
        if (z_pos_in_grid + 1 < grid_size) {
            MPI_Recv(sh_depth_B_in, sh_depth_size, MPI_CHAR, rank + grid_size * grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (z_pos_in_grid - 1 >= 0) {
            MPI_Send(sh_depth_F_out, sh_depth_size, MPI_CHAR, rank - grid_size * grid_size, 1, MPI_COMM_WORLD);
        }
    }

    if (z_pos_in_grid % 2 == 1) {
        if (z_pos_in_grid - 1 >= 0) {
            MPI_Recv(sh_depth_F_in, sh_depth_size, MPI_CHAR, rank - grid_size * grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (z_pos_in_grid + 1 < grid_size) {
            MPI_Send(sh_depth_B_out, sh_depth_size, MPI_CHAR, rank + grid_size * grid_size, 1, MPI_COMM_WORLD);
        }
    }

    if (z_pos_in_grid % 2 == 1) {
        if (z_pos_in_grid + 1 < grid_size) {
            MPI_Recv(sh_depth_B_in, sh_depth_size, MPI_CHAR, rank + grid_size * grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (z_pos_in_grid - 1 >= 0) {
            MPI_Send(sh_depth_F_out, sh_depth_size, MPI_CHAR, rank - grid_size * grid_size, 1, MPI_COMM_WORLD);
        }
    }

}

void Node::recv_buffer(bool condition, char *intput_buffer, char *output_buffer, int in_x, int in_y, int in_z) {
    if (condition) {
        if (node_in_grid(in_x, in_y, in_z)) {
            MPI_Recv(intput_buffer, 1, MPI_CHAR, rank_with_offset(in_x, in_y, in_z), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        int out_x = -in_x;
        int out_y = -in_y;
        int out_z = -in_z;
        if (node_in_grid(out_x, out_y, out_z)) {
            MPI_Send(output_buffer, 1, MPI_CHAR, rank_with_offset(out_x, out_y, out_z), 1, MPI_COMM_WORLD);
        }
    }
}

void Node::share_corners() {

    recv_buffer(z_pos_in_grid % 2 == 0, sh_corner_FUL_in, sh_corner_BDR_out, -1, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_corner_BDR_in, sh_corner_FUL_out, 1, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_corner_FUR_in, sh_corner_BDL_out, 1, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_corner_BDL_in, sh_corner_FUR_out, -1, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_corner_FDL_in, sh_corner_BUR_out, -1, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_corner_BUR_in, sh_corner_FDL_out, 1, -1, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_corner_FDR_in, sh_corner_BUL_out, 1, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_corner_BUL_in, sh_corner_FDR_out, -1, -1, 1);

    recv_buffer(z_pos_in_grid % 2 == 1, sh_corner_FUL_in, sh_corner_BDR_out, -1, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_corner_BDR_in, sh_corner_FUL_out, 1, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_corner_FUR_in, sh_corner_BDL_out, 1, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_corner_BDL_in, sh_corner_FUR_out, -1, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_corner_FDL_in, sh_corner_BUR_out, -1, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_corner_BUR_in, sh_corner_FDL_out, 1, -1, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_corner_FDR_in, sh_corner_BUL_out, 1, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_corner_BUL_in, sh_corner_FDR_out, -1, -1, 1);

}

void Node::share_edges() {

    recv_buffer(y_pos_in_grid % 2 == 0, sh_edge_UL_in, sh_edge_DR_out, -1, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 0, sh_edge_DR_in, sh_edge_UL_out, 1, 1, 0);
    recv_buffer(y_pos_in_grid % 2 == 0, sh_edge_UR_in, sh_edge_DL_out, 1, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 0, sh_edge_DL_in, sh_edge_UR_out, -1, 1, 0);

    recv_buffer(z_pos_in_grid % 2 == 0, sh_edge_FL_in, sh_edge_BR_out, -1, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_edge_BR_in, sh_edge_FL_out, 1, 0, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_edge_FR_in, sh_edge_BL_out, 1, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_edge_BL_in, sh_edge_FR_out, -1, 0, 1);

    recv_buffer(z_pos_in_grid % 2 == 0, sh_edge_FU_in, sh_edge_BD_out, 0, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_edge_BD_in, sh_edge_FU_out, 0, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_edge_FD_in, sh_edge_BU_out, 0, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 0, sh_edge_BU_in, sh_edge_FD_out, 0, -1, 1);

    recv_buffer(y_pos_in_grid % 2 == 1, sh_edge_UL_in, sh_edge_DR_out, -1, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 1, sh_edge_DR_in, sh_edge_UL_out, 1, 1, 0);
    recv_buffer(y_pos_in_grid % 2 == 1, sh_edge_UR_in, sh_edge_DL_out, 1, -1, 0);
    recv_buffer(y_pos_in_grid % 2 == 1, sh_edge_DL_in, sh_edge_UR_out, -1, 1, 0);
                                     
    recv_buffer(z_pos_in_grid % 2 == 1, sh_edge_FL_in, sh_edge_BR_out, -1, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_edge_BR_in, sh_edge_FL_out, 1, 0, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_edge_FR_in, sh_edge_BL_out, 1, 0, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_edge_BL_in, sh_edge_FR_out, -1, 0, 1);
                                     
    recv_buffer(z_pos_in_grid % 2 == 1, sh_edge_FU_in, sh_edge_BD_out, 0, -1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_edge_BD_in, sh_edge_FU_out, 0, 1, 1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_edge_FD_in, sh_edge_BU_out, 0, 1, -1);
    recv_buffer(z_pos_in_grid % 2 == 1, sh_edge_BU_in, sh_edge_FD_out, 0, -1, 1);

}

void Node::pre_share_copy() {

    char ***share_array = array[current_array_idx];

    // Vertical
    if (y_pos_in_grid + 1 < grid_size) {
        for (int z = 0; z < node_size; z++) {
            for (int y = 0; y < share_thickness; y++) {
                for (int x = 0; x < node_size; x++) {
                    sh_vertical_D_out[z * node_size * share_thickness + y * node_size + x] = share_array[share_thickness + z][node_size + y][share_thickness + x];
                }
            }
        }
    }

    if (y_pos_in_grid - 1 >= 0) {
        for (int z = 0; z < node_size; z++) {
            for (int y = 0; y < share_thickness; y++) {
                for (int x = 0; x < node_size; x++) {
                    sh_vertical_U_out[z * node_size * share_thickness + y * node_size + x] = share_array[share_thickness + z][share_thickness + y][share_thickness + x];
                }
            }
        }
    }

    // Horizontal
    if (x_pos_in_grid + 1 < grid_size) {
        for (int z = 0; z < node_size; z++) {
            for (int y = 0; y < node_size; y++) {
                for (int x = 0; x < share_thickness; x++) {
                    sh_horizontal_R_out[z * node_size * share_thickness + y * share_thickness + x] = share_array[share_thickness + z][share_thickness + y][node_size + x];
                }
            }
        }
    }

    if (x_pos_in_grid - 1 >= 0) {
        for (int z = 0; z < node_size; z++) {
            for (int y = 0; y < node_size; y++) {
                for (int x = 0; x < share_thickness; x++) {
                    sh_horizontal_L_out[z * node_size * share_thickness + y * share_thickness + x] = share_array[share_thickness + z][share_thickness + y][share_thickness + x];
                }
            }
        }
    }

    // Depth
    if (z_pos_in_grid + 1 < grid_size) {
        for (int z = 0; z < share_thickness; z++) {
            for (int y = 0; y < node_size; y++) {
                for (int x = 0; x < node_size; x++) {
                    sh_depth_B_out[z * node_size * node_size + y * node_size + x] = share_array[node_size + z][share_thickness + y][share_thickness + x];
                }
            }
        }
    }

    if (z_pos_in_grid - 1 >= 0) {
        for (int z = 0; z < share_thickness; z++) {
            for (int y = 0; y < node_size; y++) {
                for (int x = 0; x < node_size; x++) {
                    sh_depth_F_out[z * node_size * node_size + y * node_size + x] = share_array[share_thickness + z][share_thickness + y][share_thickness + x];
                }
            }
        }
    }

    // Corners
    
    // Edges

}

void Node::post_share_copy() {

    char ***share_array = array[current_array_idx];

    // Vertical
    if (y_pos_in_grid - 1 >= 0) {
        for (int z = 0; z < node_size; z++) {
            for (int y = 0; y < share_thickness; y++) {
                for (int x = 0; x < node_size; x++) {
                    share_array[share_thickness + z][y][share_thickness + x] = sh_vertical_U_in[z * node_size * share_thickness + y * node_size + x];
                }
            }
        }
    }

    if (y_pos_in_grid + 1 < grid_size) {
        for (int z = 0; z < node_size; z++) {
            for (int y = 0; y < share_thickness; y++) {
                for (int x = 0; x < node_size; x++) {
                    share_array[share_thickness + z][node_size + share_thickness + y][share_thickness + x] = sh_vertical_D_in[z * node_size * share_thickness + y * node_size + x];
                }
            }
        }
    }

    // Horizontal
    if (x_pos_in_grid - 1 >= 0) {
        for (int z = 0; z < node_size; z++) {
            for (int y = 0; y < node_size; y++) {
                for (int x = 0; x < share_thickness; x++) {
                    share_array[share_thickness + z][share_thickness + y][x] = sh_horizontal_L_in[z * node_size * share_thickness + y * share_thickness + x];
                }
            }
        }
    }

    if (x_pos_in_grid + 1 < grid_size) {
        for (int z = 0; z < node_size; z++) {
            for (int y = 0; y < node_size; y++) {
                for (int x = 0; x < share_thickness; x++) {
                    share_array[share_thickness + z][share_thickness + y][share_thickness + node_size + x] = sh_horizontal_R_in[z * node_size * share_thickness + y * share_thickness + x];
                }
            }
        }
    }

    // Depth
    if (z_pos_in_grid + 1 < grid_size) {
        for (int z = 0; z < share_thickness; z++) {
            for (int y = 0; y < node_size; y++) {
                for (int x = 0; x < node_size; x++) {
                    share_array[node_size + share_thickness + z][share_thickness + y][share_thickness + x] = sh_depth_B_in[z * node_size * node_size + y * node_size + x];
                }
            }
        }
    }

    if (z_pos_in_grid - 1 >= 0) {
        for (int z = 0; z < share_thickness; z++) {
            for (int y = 0; y < node_size; y++) {
                for (int x = 0; x < node_size; x++) {
                    share_array[z][share_thickness + y][share_thickness + x] = sh_depth_F_in[z * node_size * node_size + y * node_size + x];
                }
            }
        }
    }

    // Corners
    
    // Edges

}

void Node::iter() {

    char ***input_array = array[current_array_idx];
    char ***output_array = array[current_array_idx ^ 1];

    for (int z = share_thickness; z < node_size + share_thickness; z++) {
        for (int y = share_thickness; y < node_size + share_thickness; y++) {
            for (int x = share_thickness; x < node_size + share_thickness; x++) {

                int val = 0;

                // sum 26 neighbours
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        for (int k = -1; k <= 1; k++) {
                            if (i == 0 && j == 0 && z == 0) {
                                continue;
                            }

                            val += input_array[z + i][y + j][x + k] > 0 && input_array[z + i][y + j][x + k] < 5;
                        }
                    }
                }

                // 3D CA rules - Amoeba (9-26/5-7,12-13,15/5/M)
                if (input_array[z][y][x] == 0) {
                    if ((val >= 5 && val <= 7) || val == 12 || val == 13 || val == 15) {
                        output_array[z][y][x] = 4;
                    } else {
                        output_array[z][y][x] = 0;
                    }
                } else {
                    if (val >= 9 && val <= 26) {
                        output_array[z][y][x]--;
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

    MPI_Scatter(MPI_IN_PLACE, 0, MPI_CHAR, send_array, node_size * node_size * node_size, MPI_CHAR, 0, MPI_COMM_WORLD);

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

    MPI_Gather(send_array, node_size * node_size * node_size, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
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