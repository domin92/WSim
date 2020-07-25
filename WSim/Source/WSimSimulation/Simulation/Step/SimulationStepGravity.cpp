#include "SimulationStepGravity.hpp"

#include "Source/WSimSimulation/Simulation/Simulation.hpp"

SimulationStepGravity::SimulationStepGravity(Simulation &simulation, Vec3 &outputVelocitySize)
    : SimulationStep(simulation, outputVelocitySize, outputVelocitySize),
      kernelApplyGravity(simulation.getKernelManager()["gravity.cl"]["applyGravity"]) {}

void SimulationStepGravity::run(float deltaTimeSeconds) {
    auto force = simulation.getGravityForce();
    if (force == 0.f) {
        return;
    }

    auto &velocity = simulation.getVelocity();
    auto &color = simulation.getColor();
    auto &commandQueue = simulation.getCommandQueue();

    auto gws = outputVelocitySize;
    auto velocityOffset = calculateBorderOffset(simulation.getSimulationSizeWithBorder(), gws, simulation.getPositionInGrid());
    OCL::setKernelArgMem(kernelApplyGravity, 0, velocity.getSource());                                 // inVelocity
    OCL::setKernelArgMem(kernelApplyGravity, 1, color.getSource());                                    // inColor
    OCL::setKernelArgVec(kernelApplyGravity, 2, velocityOffset.x, velocityOffset.y, velocityOffset.z); // inVelocityOffset
    OCL::setKernelArgVec(kernelApplyGravity, 3, 0.f, 1.f, 0.f);                                        // inDownDirection
    OCL::setKernelArgFlt(kernelApplyGravity, 4, force);                                                // inGravityForce
    OCL::setKernelArgMem(kernelApplyGravity, 5, velocity.getDestinationAndSwap());                     // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelApplyGravity, gws);
}

void SimulationStepGravity::stop() {}
