#include "MasterRendererInterfaceVoxel.h"

#include "Source/WSimRenderer/VoxelRenderer.h"

#include <mpi.h>

MasterRendererInterfaceVoxel::MasterRendererInterfaceVoxel(Master &master)
    : MasterRendererInterface(master),
      rendererCallbacks(master),
      renderer(createRenderer(master)) {}

void MasterRendererInterfaceVoxel::mainLoop() {
    renderer->mainLoop();
}

std::unique_ptr<Renderer> MasterRendererInterfaceVoxel::createRenderer(Master &master) {
    auto result = new VoxelRenderer(rendererCallbacks, master.getNodeSize(), master.getGridSize(), 1000);
    return std::unique_ptr<Renderer>{result};
}

MasterRendererInterfaceVoxel::VoxelRendererCallbacksImpl::VoxelRendererCallbacksImpl(Master &master) : master(master) {}

void MasterRendererInterfaceVoxel::VoxelRendererCallbacksImpl::stepSimulation(float deltaTimeSeconds) {
    master.receiveFromNodes();
}

char **MasterRendererInterfaceVoxel::VoxelRendererCallbacksImpl::getVoxelBuffers() {
    return master.getMappedBuffer();
}
