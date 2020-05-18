#pragma once

#include "Source/WSimMPI/master/MasterRendererInterface.h"
#include "Source/WSimRenderer/VoxelRenderer.h"

class Renderer;

class MasterRendererInterfaceVoxel : public MasterRendererInterface {
public:
    MasterRendererInterfaceVoxel(Master &master);
    constexpr static size_t mainBufferTexelSize = 4 * sizeof(float);

    // For MPI
    virtual void mainLoop() override;

    // For Renderer
    class VoxelRendererCallbacksImpl : public VoxelRendererCallbacks {
    public:
        VoxelRendererCallbacksImpl(Master &master);
        void stepSimulation(float deltaTimeSeconds) override;

    private:
        Master &master;
    };

private:
    std::unique_ptr<Renderer> createRenderer(Master &master);

    VoxelRendererCallbacksImpl rendererCallbacks;
    std::unique_ptr<Renderer> renderer;
};
