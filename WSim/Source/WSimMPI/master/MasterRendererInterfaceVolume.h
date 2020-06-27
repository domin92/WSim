#pragma once

#include "Source/WSimMPI/master/MasterRendererInterface.h"
//#include "Source/WSimRenderer/VolumeRenderer.h"

class Renderer;

class MasterRendererInterfaceVolume : public MasterRendererInterface {
public:
    MasterRendererInterfaceVolume(Master &master);
    constexpr static size_t mainBufferTexelSize = 4 * sizeof(float);

    // For MPI
    virtual void mainLoop() override;

    // For Renderer
    class VolumeRendererCallbacksImpl : public VolumeRendererCallbacks {
    public:
        VolumeRendererCallbacksImpl(Master &master);
        void stepSimulation(float deltaTimeSeconds) override;
        char *getVolumeBuffers() override;

    private:
        Master &master;
    };

private:
    std::unique_ptr<Renderer> createRenderer(Master &master);

    VolumeRendererCallbacksImpl rendererCallbacks;
    std::unique_ptr<Renderer> renderer;
};
