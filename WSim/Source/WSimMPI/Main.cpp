#include "Master/Master.hpp"
#include "Node/Node.hpp"
#include "Source/WSimCommon/ArgumentParser.h"
#include "Source/WSimCommon/Logger.h"
#include "Source/WSimMPI/Utils.h"

#include <cstdlib>
#include <mpi.h>

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
    // Parse arguments
    ArgumentParser argumentParser{argc, argv};
    int full_size = argumentParser.getArgumentValue<int>({"-s", "--simulationSize"}, DEFAULT_GRID_SIZE); // Size of the edge of the simulation cube
    int blockProcessWithRank = argumentParser.getArgumentValue<int>({"-b", "--block"}, -1);              // -1 means do not block
    bool printPid = argumentParser.getArgumentValue<bool>({"-p", "--printPids"}, 0);                     // print process ids of all MPI processes

    // Verify arguments
    if (full_size <= 0) {
        return;
    }

    int my_rank, proc_count;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);

    if (printPid) {
        int myPid = getProcessId();
        auto pids = std::make_unique<int[]>(proc_count);
        MPI_Gather(&myPid, 1, MPI_INT, pids.get(), 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (my_rank == 0) {
            for (int i = 0; i < proc_count; i++) {
                std::cerr << "Process " << i << " pid = " << pids[i] << '\n';
            }
        }
    }
    if (blockProcessWithRank == my_rank) {
        std::cerr << "Process " << my_rank << " blocked at initialization. Attach debugger to " << getProcessId() << "\n";
        while (1)
            ; // for debugging purposes
    }

    Logger::createFileLogger("log_file", my_rank);

    if (my_cbrt(proc_count - 1) == 0) {
        return;
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
