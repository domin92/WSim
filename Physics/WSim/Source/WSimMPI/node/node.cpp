#include <cstdlib>
#include <mpi.h>
#include <iostream>
#include "Node.hpp"

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
    /*sh_corner_UL_in = new char[sh_corner_size];
	sh_corner_UL_out = new char[sh_corner_size];
	sh_corner_UR_in = new char[sh_corner_size];
	sh_corner_UR_out = new char[sh_corner_size];
	sh_corner_DL_in = new char[sh_corner_size];
	sh_corner_DL_out = new char[sh_corner_size];
	sh_corner_DR_in = new char[sh_corner_size];
	sh_corner_DR_out = new char[sh_corner_size];*/

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
    /*delete[] sh_corner_UL_in;
	delete[] sh_corner_UL_out;
	delete[] sh_corner_UR_in;
	delete[] sh_corner_UR_out;
	delete[] sh_corner_DL_in;
	delete[] sh_corner_DL_out;
	delete[] sh_corner_DR_in;
	delete[] sh_corner_DR_out;*/

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
    // Nothing to share
}

void Node::share_corners() {

    /*if(y_pos_in_grid%2==0){ // Upper Left
		if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid - 1 >= 0){
			MPI_Recv(sh_corner_UL_in, 1, MPI_CHAR, rank - grid_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid + 1 < grid_size){
			MPI_Send(sh_corner_DR_out, 1, MPI_CHAR, rank + grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(y_pos_in_grid%2==0){ // Upper Right
		if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid + 1 < grid_size){
			MPI_Recv(sh_corner_UR_in, 1, MPI_CHAR, rank - grid_size + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid - 1 >= 0){
			MPI_Send(sh_corner_DL_out, 1, MPI_CHAR, rank + grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(y_pos_in_grid%2==0){ // Bottom Left
		if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid - 1 >= 0){
			MPI_Recv(sh_corner_DL_in, 1, MPI_CHAR, rank + grid_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid + 1 < grid_size){
			MPI_Send(sh_corner_UR_out, 1, MPI_CHAR, rank - grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(y_pos_in_grid%2==0){ // Bottom Right
		if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid + 1 < grid_size){
			MPI_Recv(sh_corner_DR_in, 1, MPI_CHAR, rank + grid_size + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid - 1 >= 0){
			MPI_Send(sh_corner_UL_out, 1, MPI_CHAR, rank - grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}

	if(y_pos_in_grid%2==1){ // Upper Left
		if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid - 1 >= 0){
			MPI_Recv(sh_corner_UL_in, 1, MPI_CHAR, rank - grid_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid + 1 < grid_size){
			MPI_Send(sh_corner_DR_out, 1, MPI_CHAR, rank + grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(y_pos_in_grid%2==1){ // Upper Right
		if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid + 1 < grid_size){
			MPI_Recv(sh_corner_UR_in, 1, MPI_CHAR, rank - grid_size + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid - 1 >= 0){
			MPI_Send(sh_corner_DL_out, 1, MPI_CHAR, rank + grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(y_pos_in_grid%2==1){ // Bottom Left
		if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid - 1 >= 0){
			MPI_Recv(sh_corner_DL_in, 1, MPI_CHAR, rank + grid_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid + 1 < grid_size){
			MPI_Send(sh_corner_UR_out, 1, MPI_CHAR, rank - grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(y_pos_in_grid%2==1){ // Bottom Right
		if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid + 1 < grid_size){
			MPI_Recv(sh_corner_DR_in, 1, MPI_CHAR, rank + grid_size + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid - 1 >= 0){
			MPI_Send(sh_corner_UL_out, 1, MPI_CHAR, rank - grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}*/
}

void Node::share() {
    share_vertical();
    share_horizontal();
    share_depth();
    //share_corners();
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
    /**if(y_pos_in_grid + 1 < grid_size){
		for(int i=0;i<node_size;i++){
			sh_vertical_D_out[i] = share_array[node_size][i+1];
		}
	}
	if(y_pos_in_grid - 1 >= 0){
		for(int i=0;i<node_size;i++){
			sh_vertical_U_out[i] = share_array[1][i+1];
		}
	}*/

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
    /*if(x_pos_in_grid + 1 < grid_size){
		for(int i=0;i<node_size;i++){
			sh_horizontal_R_out[i] = share_array[i + 1][node_size];
		}
	}

	if(x_pos_in_grid - 1 >= 0){
		for(int i=0;i<node_size;i++){
			sh_horizontal_L_out[i] = share_array[i + 1][1];
		}
	}*/

    // Corners
    /*if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid + 1 < grid_size){
		sh_corner_DR_out[0] = share_array[node_size][node_size];
	}

	if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid - 1 >= 0){
		sh_corner_DL_out[0] = share_array[node_size][1];
	}

	if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid + 1 < grid_size){
		sh_corner_UR_out[0] = share_array[1][node_size];
	}

	if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid - 1 >= 0){
		sh_corner_UL_out[0] = share_array[1][1];
	}*/
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
    /*if(y_pos_in_grid - 1 >= 0){
		for(int i=0;i<node_size;i++){
			share_array[0][i+1] = sh_vertical_U_in[i];
		}
	}
	if(y_pos_in_grid + 1 < grid_size){
		for(int i=0;i<node_size;i++){
			share_array[node_size + 1][i+1] = sh_vertical_D_in[i];
		}
	}*/

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
                    share_array[share_thickness + z][share_thickness + y][share_thickness + node_size + x] = sh_horizontal_L_in[z * node_size * share_thickness + y * share_thickness + x];
                }
            }
        }
    }
    /*if(x_pos_in_grid - 1 >= 0){
		for(int i=0;i<node_size;i++){
			share_array[i + 1][0] = sh_horizontal_L_in[i];
		}
	}	

	if(x_pos_in_grid + 1 < grid_size){
		for(int i=0;i<node_size;i++){
			share_array[i + 1][node_size + 1] = sh_horizontal_R_in[i];
		}
	}*/

    // Corners
    /*if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid - 1 >= 0){
		share_array[0][0] = sh_corner_UL_in[0];
	}

	if(y_pos_in_grid - 1 >= 0 && x_pos_in_grid + 1 < grid_size){
		share_array[0][node_size + 1] = sh_corner_UR_in[0];
	}

	if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid - 1 >= 0){
		share_array[node_size + 1][0] = sh_corner_DL_in[0];
	}

	if(y_pos_in_grid + 1 < grid_size && x_pos_in_grid + 1 < grid_size){
		share_array[node_size + 1][node_size + 1] = sh_corner_DR_in[0];
	}*/
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
							if (i==0 && j==0 && z==0) {
								continue;
							}

							val += input_array[z+i][y+j][x+k] > 0;
						}
					}
				}

				// 3D CA rules - Amoeba (9-26/5-7,12-13,15/5/M)
				if (input_array[z][y][x] == 0) {
					if ((val >= 5 && val <=7) || val == 12 || val == 13 || val == 15 ) {
						output_array[z][y][x] = 4;
					}
					else {
						output_array[z][y][x] = 0;
					}
				}
				else {
					if (val >= 9 && val <= 26) {
						output_array[z][y][x]--;
					}
					else {
						output_array[z][y][x] = 0;
					}
				}

                /*for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        if (i == 0 && j == 0) {
                            continue;
                        }

                        val += input_array[z][y + i][x + j] > 0;
                    }
                }

                if (input_array[z][y][x] == 0) {
                    if (val == 3) {
                        output_array[z][y][x] = 1;
                    } else {
                        output_array[z][y][x] = 0;
                    }
                } else {
                    if (val == 2 || val == 3) {
                        output_array[z][y][x] = 1;
                    } else {
                        output_array[z][y][x] = 0;
                    }
                }*/
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

void Node::main() {
    receive_from_master();
    while (true) {
        //pre_share_copy();
        //share();
        //post_share_copy();
        iter();
        send_to_master();
    }
}