#pragma once

#include "Source/WSimRenderer/VolumeRenderer.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

class VolumeRendererCallbacksImpl : public VolumeRendererCallbacks {
public:
    VolumeRendererCallbacksImpl(Simulation &simulation)
        : simulation(simulation),
          voxelBuffer(std::make_unique<uint8_t[]>(simulation.getSimulationSize().getRequiredBufferSize(Simulation::colorVoxelSize))) {
    }

    void stepSimulation(float deltaTimeSeconds) override {
        simulation.stepSimulation(deltaTimeSeconds);
    }

    uint8_t *getVolumeBuffers() override {
        const Vec3 offset = simulation.getBorderOffset();
        const Vec3 size = simulation.getSimulationSize();
        OCL::enqueueReadImage3D(simulation.getCommandQueue(), simulation.getColor().getSource(), CL_BLOCKING, offset, size, voxelBuffer.get());
        return voxelBuffer.get();
    }

private:
    Simulation &simulation;
    std::unique_ptr<uint8_t[]> voxelBuffer;
};
