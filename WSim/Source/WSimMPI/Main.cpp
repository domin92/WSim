#include <mpi.h>
#include <cstdlib>

#include "Master/Master.hpp"
#include "Node/Node.hpp"
#include "Source/WSimCommon/Logger.h"

#define DEFAULT_GRID_SIZE 60

using namespace std;

int my_cbrt(int a) {
    switch (a) {
    case 1:
        return 1;
    case 8:
        return 2;
    case 27:
        return 3;
    case 64:
        return 4;
    case 125:
        return 5;
    default:
        return 0;
    }
}

void main(int argc, char **argv) {

    int my_rank, proc_count;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);

    Logger::createFileLogger("log_file", my_rank);

    if (my_cbrt(proc_count - 1) == 0) {
        return;
    }

    int full_size = DEFAULT_GRID_SIZE; // Size of the edge of the simulation cube

    if (argc > 1) {
        full_size = atoi(argv[1]);
        if (full_size <= 0) {
            return;
        }
    }

    int grid_size = my_cbrt(proc_count - 1);
    int node_size = full_size / grid_size;

    if (my_rank == 0) {
        Master master(proc_count, grid_size, node_size);
        master.main();
    } else {
        Node node(my_rank, grid_size, node_size);
        node.main();
    }

    MPI_Finalize();
}