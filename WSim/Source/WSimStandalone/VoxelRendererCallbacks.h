#pragma once

#include "Source/WSimRenderer/VoxelRenderer.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"

class VoxelRendererCallbacksImpl : public VoxelRendererCallbacks {
public:
    VoxelRendererCallbacksImpl(Simulation &simulation)
        : simulation(simulation),
          voxelBuffer(std::make_unique<char[]>(simulation.getSimulationSize().getRequiredBufferSize(4 * sizeof(float)))) {
        voxelBuffers[0] = voxelBuffer.get();
    }

    void stepSimulation(float deltaTimeSeconds) override {
        simulation.stepSimulation(deltaTimeSeconds);
    }

    char **getVoxelBuffers() override {
        const Vec3 offset = simulation.getBorderOffset();
        const Vec3 size = simulation.getSimulationSize();
        OCL::enqueueReadImage3D(simulation.getCommandQueue(), simulation.getColor().getSource(), CL_BLOCKING, offset, size, voxelBuffer.get());
        return voxelBuffers;
    }

private:
    Simulation &simulation;
    std::unique_ptr<char[]> voxelBuffer;
    char* voxelBuffers[1] = {};
};
