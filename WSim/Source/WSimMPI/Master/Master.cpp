#include "Master.hpp"

#include "Source/WSimCommon/SimulationMode.h"
#include "Source/WSimMPI/Master/MasterRendererInterface2D.hpp"
#include "Source/WSimMPI/Master/MasterRendererInterface3D.hpp"
#include "Source/WSimMPI/Master/MasterRendererInterfaceText.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mpi.h>

Master::Master(int procCount, int gridSizeInNodes, int nodeSize, SimulationMode simulationMode)
    : procCount(procCount),
      gridSize(gridSizeInNodes),
      nodeSize(nodeSize),
      fullSize(nodeSize * gridSizeInNodes),
      nodeVolume(nodeSize * nodeSize * nodeSize * Simulation::colorVoxelSize),
      mainBuffer(new uint8_t[(procCount - 1) * nodeVolume]),
      mappedBuffer(new uint8_t *[procCount - 1]),
      rendererInterface(createRendererInterface(simulationMode)) {
    for (int i = 0; i < procCount - 1; i++) {
        mappedBuffer[i] = mainBuffer + i * nodeVolume;
    }
}

std::unique_ptr<MasterRendererInterface> Master::createRendererInterface(SimulationMode simulationMode) {
    switch (simulationMode.value) {
    case SimulationMode::Enum::Graphical2D:
        return std::unique_ptr<MasterRendererInterface>{new MasterRendererInterface2D(*this)};
    case SimulationMode::Enum::Graphical3D:
        return std::unique_ptr<MasterRendererInterface>{new MasterRendererInterface3D(*this)};
    case SimulationMode::Enum::Text:
        return std::unique_ptr<MasterRendererInterface>{new MasterRendererInterfaceText(*this)};
    default:
        wsimError();
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
                int offset = idx * nodeSize * nodeSize * nodeSize + z_in_node * nodeSize * nodeSize + y_in_node * nodeSize + x_in_node;

                if (y > (4 * fullSize / 10) && y < (5 * fullSize / 10)) {
                    reinterpret_cast<float *>(mainBuffer)[offset] = 1.0f;
                } else {
                    reinterpret_cast<float *>(mainBuffer)[offset] = 0.0f;
                }
            }
        }
    }

    // Initialize values in nodes and start rendering
    sendToNodes();
    rendererInterface->mainLoop();
}
