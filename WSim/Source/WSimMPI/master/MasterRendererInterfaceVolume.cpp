#include "MasterRendererInterfaceVolume.h"

//#include "Source/WSimRenderer/VolumeRenderer.h"

#include <mpi.h>

struct FpsCallback {
    using Clock = std::chrono::steady_clock;
    Clock::time_point lastPrintTime = {};

    void operator()(unsigned int fps) {
        const Clock::time_point now = Clock::now();
        if (now - lastPrintTime > std::chrono::milliseconds(70000)) {
            Logger::get() << "FPS: " << fps << '\n';
            lastPrintTime = now;
        }
    }
};

MasterRendererInterfaceVolume::MasterRendererInterfaceVolume(Master &master)
    : MasterRendererInterface(master),
      rendererCallbacks(master),
      renderer(createRenderer(master)) {}

void MasterRendererInterfaceVolume::mainLoop() {
    DefaultFpsCounter fpsCounter;
    FpsCallback fpsCallback;
    using Clock = std::chrono::steady_clock;
    auto lastFrameTime = Clock::now();
    while (true) {
        const auto currentFrameTime = Clock::now();
        const auto deltaTime = currentFrameTime - lastFrameTime;
        fpsCounter.push(deltaTime);
        fpsCallback(fpsCounter.getFps());
        lastFrameTime = currentFrameTime;

        rendererCallbacks.stepSimulation(std::chrono::duration_cast<std::chrono::duration<float>>(deltaTime).count());
    }
}

std::unique_ptr<Renderer> MasterRendererInterfaceVolume::createRenderer(Master &master) {
    return {};
}

MasterRendererInterfaceVolume::VolumeRendererCallbacksImpl::VolumeRendererCallbacksImpl(Master &master) : master(master) {}

void MasterRendererInterfaceVolume::VolumeRendererCallbacksImpl::stepSimulation(float deltaTimeSeconds) {
    master.receiveFromNodes();
}

char *MasterRendererInterfaceVolume::VolumeRendererCallbacksImpl::getVolumeBuffers() {
    return master.getMainBuffer();
}
