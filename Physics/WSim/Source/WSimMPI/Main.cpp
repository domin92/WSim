#include <iostream>
#include <mpi/mpi.h>

int main(int argc, char **argv) {
    int rank, processCount;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    std::cout << "Hello from process " << (rank + 1) << " of " << processCount << "\n";

    MPI_Finalize();
}
