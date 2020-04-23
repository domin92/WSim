#include<cstdlib>
#include<mpi.h>
#include<iostream>

#include "master.hpp"

Master::Master(int proc_count, int grid_size, int node_size) {

    this->proc_count = proc_count;
    this->grid_size = grid_size;
    this->node_size = node_size;

    node_volume = node_size * node_size * node_size;

    main_buffer = new char[proc_count*node_volume];

    mapped_buffer = new char *[proc_count - 1];

    for (int i = 0; i < proc_count - 1; i++) {
        mapped_buffer[i] = main_buffer + (i + 1) * node_volume;
    }
}

Master::~Master() {
    delete[] mapped_buffer;
    delete[] main_buffer;
}

void Master::send_to_nodes() {
    MPI_Scatter(main_buffer, node_volume, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Master::receive_from_nodes() {
    MPI_Gather(MPI_IN_PLACE, 0, MPI_CHAR, main_buffer, node_volume, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Master::main() {

    for (int z = 0; z < node_size*grid_size; z++) {
        for (int y = 0; y < node_size * grid_size; y++) {
            for (int x = 0; x < node_size * grid_size; x++) {

                int z_in_node = z % node_size;
                int y_in_node = y % node_size;
                int x_in_node = x % node_size;

                int z_in_grid = z / node_size;
                int y_in_grid = y / node_size;
                int x_in_grid = x / node_size;

                int idx = z_in_grid * grid_size * grid_size + y_in_grid * grid_size + x_in_grid;

                int r = rand() % 100;

                if (r > 50) {
                    mapped_buffer[idx][z_in_node * node_size * node_size + y_in_node * node_size + x_in_node] = 1;
                }
            }
        }
    }

    send_to_nodes();

    while (true) {

        receive_from_nodes();

        for (int i = 0; i < /*grid_size*/1; i++) {
            for (int j = 0; j < grid_size; j++) {
                for (int k = 0; k < grid_size; k++) {

                    for (int z = 0; z < /*node_size*/1; z++) {
                        for (int y = 0; y < node_size; y++) {
                            for (int x = 0; x < node_size; x++) {

                                int idx = i * grid_size * grid_size + j * grid_size + k;

                                int power = mapped_buffer[idx][z * node_size * node_size + y * node_size + x];

                                int color = ((63 * power) / (grid_size * node_size + 60)) * ((i * node_size) + z + 60);

                                if (power > 0) {
                                    std::cout << "#";
                                } else {
                                    std::cout << " ";
                                }
                            }
                            std::cout << "\n";
                        }
                    }
                }
            }
        }

    }
}
