#include "MasterRendererInterfaceVoxel.h"

#include "Source/WSimRenderer/VoxelRenderer.h"

#include <mpi.h>

MasterRendererInterfaceVoxel::MasterRendererInterfaceVoxel(Master &master)
    : MasterRendererInterface(master, createRenderer(master)) {}

std::unique_ptr<Renderer> MasterRendererInterfaceVoxel::createRenderer(Master &master) {
    auto result = new VoxelRenderer(*this, master.getNodeSize(), master.getGridSize(), 1000, master.getMappedBuffer());
    return std::unique_ptr<Renderer>{result};
}
