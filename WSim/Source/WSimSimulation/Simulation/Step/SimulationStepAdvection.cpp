#include "SimulationStepAdvection.hpp"

#include "Source/WSimSimulation/Simulation/Simulation.hpp"

SimulationStepAdvection::SimulationStepAdvection(Simulation &simulation, Vec3 &outputVelocitySize)
    : SimulationStep(simulation, outputVelocitySize, simulation.getSimulationSizeWithBorder()),
      kernelAdvection(simulation.getKernelManager()["advection.cl"]["advectVelocity"]) {}

void SimulationStepAdvection::run(float deltaTimeSeconds) {
    auto &velocity = simulation.getVelocity();
    auto &obstacles = simulation.getObstacles();
    auto &commandQueue = simulation.getCommandQueue();

    auto gws = outputVelocitySize;
    auto velocityOffset = calculateBorderOffset(simulation.getSimulationSizeWithBorder(), gws, simulation.getPositionInGrid());
    OCL::setKernelArgMem(kernelAdvection, 0, velocity.getSource());                                 // inVelocity
    OCL::setKernelArgMem(kernelAdvection, 1, obstacles);                                            // inObstacles
    OCL::setKernelArgVec(kernelAdvection, 2, velocityOffset.x, velocityOffset.y, velocityOffset.z); // inVelocityOffset
    OCL::setKernelArgFlt(kernelAdvection, 3, deltaTimeSeconds);                                     // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 4, 1.f);                                                  // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 5, velocity.getDestinationAndSwap());                     // outField
    OCL::enqueueKernel3D(commandQueue, kernelAdvection, outputVelocitySize);
}

void SimulationStepAdvection::stop() {}
