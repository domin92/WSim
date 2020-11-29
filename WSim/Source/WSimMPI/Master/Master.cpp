#include "Master.hpp"

#include "Source/WSimCommon/SimulationMode.h"
#include "Source/WSimMPI/Master/MasterRendererInterface2D.hpp"
#include "Source/WSimMPI/Master/MasterRendererInterface3D.hpp"
#include "Source/WSimMPI/Master/MasterRendererInterfaceLevelSet3D.hpp"
#include "Source/WSimMPI/Master/MasterRendererInterfaceText.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mpi.h>

Master::Master(int procCount, int gridSizeInNodes, int nodeSize, SimulationMode simulationMode, bool benchmark)
    : procCount(procCount),
      gridSize(gridSizeInNodes),
      nodeSize(nodeSize),
      fullSize(nodeSize * gridSizeInNodes),
      nodeVolume(nodeSize * nodeSize * nodeSize * Simulation::colorVoxelSize),
      mainBuffer(new uint8_t[(procCount - 1) * nodeVolume]),
      mappedBuffer(new uint8_t *[procCount - 1]),
      simulationMode(simulationMode),
      rendererInterface(createRendererInterface(simulationMode)),
      benchmark(benchmark) {
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
    case SimulationMode::Enum::LevelSet3D:
        return std::unique_ptr<MasterRendererInterface>{new MasterRendererInterfaceLevelSet3D(*this, benchmark)};
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
    MPI_Gather(MPI_IN_PLACE, 0, MPI_CHAR, mainBuffer - nodeVolume, nodeVolume, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Master::main() {
    rendererInterface->mainLoop();
}
