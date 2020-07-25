#include "MasterRendererInterfaceVolume.hpp"

#include "Source/WSimRenderer/VolumeRenderer.hpp"

#include <mpi.h>

MasterRendererInterface3D::MasterRendererInterface3D(Master &master)
    : MasterRendererInterface(master),
      rendererCallbacks(master),
      renderer(createRenderer(master)) {}

void MasterRendererInterface3D::mainLoop() {
    renderer->mainLoop();
}

std::unique_ptr<Renderer> MasterRendererInterface3D::createRenderer(Master &master) {
    auto result = new VolumeRenderer(rendererCallbacks, master.getNodeSize(), master.getGridSize(), 1000);
    return std::unique_ptr<Renderer>{result};
}

MasterRendererInterface3D::VolumeRendererCallbacksImpl::VolumeRendererCallbacksImpl(Master &master) : master(master) {}

void MasterRendererInterface3D::VolumeRendererCallbacksImpl::stepSimulation(float deltaTimeSeconds) {
    master.receiveFromNodes();
}

char *MasterRendererInterface3D::VolumeRendererCallbacksImpl::getVolumeBuffers() {
    return master.getMainBuffer();
}
