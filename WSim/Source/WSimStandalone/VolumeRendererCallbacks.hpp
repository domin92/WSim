#pragma once

#include "Source/WSimRenderer/VolumeRenderer.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

class VolumeRendererCallbacksImpl : public VolumeRendererCallbacks {
public:
    VolumeRendererCallbacksImpl(Simulation &simulation)
        : simulation(simulation),
          voxelBuffer(std::make_unique<char[]>(simulation.getSimulationSize().getRequiredBufferSize(4 * sizeof(float)))) {
    }

    void stepSimulation(float deltaTimeSeconds) override {
        simulation.stepSimulation(deltaTimeSeconds);
    }

    char *getVolumeBuffers() override {
        const Vec3 offset = simulation.getBorderOffset();
        const Vec3 size = simulation.getSimulationSize();
        OCL::enqueueReadImage3D(simulation.getCommandQueue(), simulation.getColor().getSource(), CL_BLOCKING, offset, size, voxelBuffer.get());
        return voxelBuffer.get();
    }

private:
    Simulation &simulation;
    std::unique_ptr<char[]> voxelBuffer;
};
