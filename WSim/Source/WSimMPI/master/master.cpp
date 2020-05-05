#include "Master.hpp"

#include "Source/WSimMPI/master/MasterRendererInterfaceVoxel.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mpi.h>

Master::Master(int proc_count, int grid_size, int node_size)
    : proc_count(proc_count),
      grid_size(grid_size),
      node_size(node_size),
      full_size(node_size * grid_size),
      node_volume(MasterRendererInterfaceVoxel::mainBufferTexelSize * node_size * node_size * node_size),
      main_buffer(new char[(proc_count - 1) * node_volume]),
      mapped_buffer(new char *[proc_count - 1]),
      rendererInterface(new MasterRendererInterfaceVoxel(*this)) {
    for (int i = 0; i < proc_count - 1; i++) {
        mapped_buffer[i] = main_buffer + i * node_volume;
    }
}

Master::~Master() {
    delete[] mapped_buffer;
    delete[] main_buffer;
    MPI_Abort(MPI_COMM_WORLD, 0);
}

void Master::sendToNodes() {
    MPI_Scatter(main_buffer - node_volume, node_volume, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
    rendererInterface->sendToNodesExtra();
}

void Master::receiveFromNodes() {
    MPI_Gather(MPI_IN_PLACE, 0, MPI_CHAR, main_buffer - node_volume, node_volume, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Master::main() {
    // Initialize main buffer
    for (int z = 0; z < full_size; z++) {
        for (int y = 0; y < full_size; y++) {
            for (int x = 0; x < full_size; x++) {

                int z_in_node = z % node_size;
                int y_in_node = y % node_size;
                int x_in_node = x % node_size;

                int z_in_grid = z / node_size;
                int y_in_grid = y / node_size;
                int x_in_grid = x / node_size;

                int idx = z_in_grid * grid_size * grid_size + y_in_grid * grid_size + x_in_grid;

                int r = rand() % 100;

                if (r > 50) {
                    mapped_buffer[idx][z_in_node * node_size * node_size + y_in_node * node_size + x_in_node] = 1; // + r%4;
                } else {
                    mapped_buffer[idx][z_in_node * node_size * node_size + y_in_node * node_size + x_in_node] = 0;
                }
            }
        }
    }

    // Initialize values in nodes and start rendering
    sendToNodes();
    rendererInterface->mainLoop();
}
