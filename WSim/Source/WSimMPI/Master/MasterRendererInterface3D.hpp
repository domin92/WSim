#pragma once

#include "Source/WSimMPI/Master/MasterRendererInterface.hpp"
#include "Source/WSimRenderer/VolumeRenderer.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

class Renderer;

class MasterRendererInterface3D : public MasterRendererInterface {
public:
    MasterRendererInterface3D(Master &master);
    constexpr static size_t colorVoxelSize = Simulation::colorVoxelSize;

    // For MPI
    virtual void mainLoop() override;

    // For Renderer
    class VolumeRendererCallbacksImpl : public VolumeRendererCallbacks {
    public:
        VolumeRendererCallbacksImpl(Master &master);
        void stepSimulation(float deltaTimeSeconds) override;
        uint8_t *getVolumeBuffers() override;

    private:
        Master &master;
    };

private:
    std::unique_ptr<Renderer> createRenderer(Master &master);

    VolumeRendererCallbacksImpl rendererCallbacks;
    std::unique_ptr<Renderer> renderer;
};
