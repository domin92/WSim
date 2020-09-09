#include "Master.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mpi.h>

#ifdef WSIM_TEXT_ONLY
#include "Source/WSimMPI/Master/MasterRendererInterfaceText.hpp"
using UsedRendererInterface = MasterRendererInterfaceText;
#else
#include "Source/WSimMPI/Master/MasterRendererInterface3D.hpp"
using UsedRendererInterface = MasterRendererInterface3D;
#endif

Master::Master(int procCount, int grid_size, int nodeSize)
    : procCount(procCount),
      gridSize(grid_size),
      nodeSize(nodeSize),
      fullSize(nodeSize * grid_size),
      nodeVolume(UsedRendererInterface::mainBufferTexelSize * nodeSize * nodeSize * nodeSize),
      mainBuffer(new char[(procCount - 1) * nodeVolume]),
      mappedBuffer(new char *[procCount - 1]),
      rendererInterface(new UsedRendererInterface(*this)) {
    for (int i = 0; i < procCount - 1; i++) {
        mappedBuffer[i] = mainBuffer + i * nodeVolume;
    }
}

Master::~Master() {
    delete[] mappedBuffer;
    delete[] mainBuffer;
    MPI_Abort(MPI_COMM_WORLD, 0);
}

void Master::sendToNodes() {
    MPI_Scatter(mainBuffer - nodeVolume, nodeVolume, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
    rendererInterface->sendToNodesExtra();
}

void Master::receiveFromNodes() {
    MPI_Request r;
    MPI_Igather(MPI_IN_PLACE, 0, MPI_CHAR, mainBuffer - nodeVolume, nodeVolume, MPI_CHAR, 0, MPI_COMM_WORLD, &r);
    MPI_Wait(&r, MPI_STATUS_IGNORE);
}

void Master::main() {
    // Initialize main buffer
    for (int z = 0; z < fullSize; z++) {
        for (int y = 0; y < fullSize; y++) {
            for (int x = 0; x < fullSize; x++) {

                int z_in_node = z % nodeSize;
                int y_in_node = y % nodeSize;
                int x_in_node = x % nodeSize;

                int z_in_grid = z / nodeSize;
                int y_in_grid = y / nodeSize;
                int x_in_grid = x / nodeSize;

                int idx = z_in_grid * gridSize * gridSize + y_in_grid * gridSize + x_in_grid;

                if (y < (1 * fullSize / 10)) {
                    ((float *)mainBuffer)[(idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node) * 4] = 0.0f;
                    ((float *)mainBuffer)[(idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node) * 4 + 1] = 0.0f;
                    ((float *)mainBuffer)[(idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node) * 4 + 2] = 1.0f;
                    ((float *)mainBuffer)[(idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node) * 4 + 3] = 1.0f;
                } else {
                    ((float *)mainBuffer)[(idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node) * 4] = 0.0f;
                    ((float *)mainBuffer)[(idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node) * 4 + 1] = 0.0f;
                    ((float *)mainBuffer)[(idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node) * 4 + 2] = 0.0f;
                    ((float *)mainBuffer)[(idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node) * 4 + 3] = 1.0f;
                }
            }
        }
    }

    // Initialize values in nodes and start rendering
    sendToNodes();
    rendererInterface->mainLoop();
}
