#include "MasterRendererInterfaceLevelSet3D.hpp"

#include <mpi.h>

MasterRendererInterfaceLevelSet3D::MasterRendererInterfaceLevelSet3D(Master &master)
    : MasterRendererInterface(master),
      rendererCallbacks(master),
      renderer(createRenderer(master)) {}

void MasterRendererInterfaceLevelSet3D::mainLoop() {
    renderer->mainLoop();
}

std::unique_ptr<Renderer> MasterRendererInterfaceLevelSet3D::createRenderer(Master &master) {
    auto result = new LevelSetRenderer(rendererCallbacks, 600, 600, master.getNodeSize(), master.getGridSize());
    return std::unique_ptr<Renderer>{result};
}

MasterRendererInterfaceLevelSet3D::LevelSetRendererCallbacksImpl::LevelSetRendererCallbacksImpl(Master &master)
    : master(master) {}

void MasterRendererInterfaceLevelSet3D::LevelSetRendererCallbacksImpl::updateSimulation(float deltaTimeSeconds) {
    master.receiveFromNodes();
}

float *MasterRendererInterfaceLevelSet3D::LevelSetRendererCallbacksImpl::getData() {
    return reinterpret_cast<float *>(master.getMainBuffer());
}
