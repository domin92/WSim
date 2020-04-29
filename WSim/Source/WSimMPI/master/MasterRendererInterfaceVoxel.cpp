#include "MasterRendererInterfaceVoxel.h"

#include "Source/WSimRenderer/VoxelRenderer.h"

#include <mpi.h>

MasterRendererInterfaceVoxel::MasterRendererInterfaceVoxel(Master &master)
    : MasterRendererInterface(master, createRenderer(master)) {}

std::unique_ptr<Renderer> MasterRendererInterfaceVoxel::createRenderer(Master &master) {
    auto result = new VoxelRenderer(*this, master.getNodeSize(), master.getGridSize(), 1000, master.getMappedBuffer());
    return std::unique_ptr<Renderer>{result};
}

void MasterRendererInterfaceVoxel::sendToNodes() {
    MPI_Scatter(master.getMainBuffer() - master.getNodeVolume(), master.getNodeVolume(), MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void MasterRendererInterfaceVoxel::stepSimulation(float dt) {
    MPI_Gather(MPI_IN_PLACE, 0, MPI_CHAR, master.getMainBuffer() - master.getNodeVolume(), master.getNodeVolume(), MPI_CHAR, 0, MPI_COMM_WORLD);
}
