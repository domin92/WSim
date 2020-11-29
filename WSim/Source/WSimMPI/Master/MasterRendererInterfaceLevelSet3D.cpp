#include "MasterRendererInterfaceLevelSet3D.hpp"

#include <mpi.h>

MasterRendererInterfaceLevelSet3D::MasterRendererInterfaceLevelSet3D(Master &master, bool benchmark)
    : MasterRendererInterface(master),
      rendererCallbacks(master),
      renderer(createRenderer(master, benchmark)) {}

void MasterRendererInterfaceLevelSet3D::mainLoop() {
    renderer->mainLoop();
}

std::unique_ptr<Renderer> MasterRendererInterfaceLevelSet3D::createRenderer(Master &master, bool benchmark) {
    auto result = new LevelSetRenderer(rendererCallbacks, 800, 800, master.getNodeSize(), master.getGridSize(), benchmark);
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
