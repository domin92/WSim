#include "MasterRendererInterfaceVolume.h"

#include "Source/WSimRenderer/VolumeRenderer.h"

#include <mpi.h>

MasterRendererInterfaceVolume::MasterRendererInterfaceVolume(Master &master)
    : MasterRendererInterface(master),
      rendererCallbacks(master),
      renderer(createRenderer(master)) {}

void MasterRendererInterfaceVolume::mainLoop() {
    renderer->mainLoop();
}

std::unique_ptr<Renderer> MasterRendererInterfaceVolume::createRenderer(Master &master) {
    auto result = new VolumeRenderer(rendererCallbacks, master.getNodeSize(), master.getGridSize(), 1000);
    return std::unique_ptr<Renderer>{result};
}

MasterRendererInterfaceVolume::VolumeRendererCallbacksImpl::VolumeRendererCallbacksImpl(Master &master) : master(master) {}

void MasterRendererInterfaceVolume::VolumeRendererCallbacksImpl::stepSimulation(float deltaTimeSeconds) {
    master.receiveFromNodes();
}

char *MasterRendererInterfaceVolume::VolumeRendererCallbacksImpl::getVolumeBuffers() {
    return master.getMainBuffer();
}
