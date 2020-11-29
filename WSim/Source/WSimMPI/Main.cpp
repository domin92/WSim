#include "Master/Master.hpp"
#include "Node/Node.hpp"
#include "Source/WSimCommon/ArgumentParser.hpp"
#include "Source/WSimCommon/Logger.hpp"
#include "Source/WSimCommon/SimulationMode.h"
#include "Source/WSimMPI/Utils.hpp"

#include <cstdlib>
#include <mpi.h>
#include <sstream>

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

std::string getLogFileNameSuffix(int rank, int gridSize) {
    if (rank == 0) {
        return "0_master";
    }

    const auto x = convertTo3DRankX(rank, gridSize);
    const auto y = convertTo3DRankY(rank, gridSize);
    const auto z = convertTo3DRankZ(rank, gridSize);
    std::ostringstream result{};
    result << rank << "_node_" << x << y << z;
    return result.str();
}

int main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser argumentParser{argc, argv};
    int fullSize = argumentParser.getArgumentValue<int>({"-s", "--simulationSize"}, DEFAULT_GRID_SIZE);                             // Size of the edge of the simulation cube
    int blockProcessWithRank = argumentParser.getArgumentValue<int>({"-d", "--debug"}, -1);                                         // -1 means do not block
    bool printPid = argumentParser.getArgumentValue<bool>({"-p", "--printPids"}, 0);                                                // print process ids of all MPI processes
    auto simulationMode = SimulationMode::fromString(argumentParser.getArgumentValue<std::string>({"-m", "--mode"}, "levelset3d")); // select type of the simulation
    bool benchmark = argumentParser.getArgumentValue<bool>({"-b", "--benchmark"}, 0);                                               // print FPS

    // Verify arguments
    if (fullSize <= 0) {
        return 1;
    }

    int my_rank, procCount;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    if (printPid) {
        int myPid = getProcessId();
        auto pids = std::make_unique<int[]>(procCount);
        MPI_Gather(&myPid, 1, MPI_INT, pids.get(), 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (my_rank == 0) {
            for (int i = 0; i < procCount; i++) {
                std::cerr << "Process " << i << " pid = " << pids[i] << '\n';
            }
        }
    }
    if (blockProcessWithRank == my_rank) {
        std::cerr << "Process " << my_rank << " blocked at initialization. Attach debugger to " << getProcessId() << "\n";
        while (1)
            ; // for debugging purposes
    }

    if (my_cbrt(procCount - 1) == 0) {
        return 1;
    }
    int gridSize = my_cbrt(procCount - 1);
    int nodeSize = fullSize / gridSize;

    Logger::createFileLogger("log_file", getLogFileNameSuffix(my_rank, gridSize));

    if (my_rank == 0) {
        Master master(procCount, gridSize, nodeSize, simulationMode.get()->value, benchmark);
        master.main();
    } else {
        Node node(my_rank, gridSize, nodeSize, *simulationMode, benchmark);
        node.main();
    }

    MPI_Finalize();
}
