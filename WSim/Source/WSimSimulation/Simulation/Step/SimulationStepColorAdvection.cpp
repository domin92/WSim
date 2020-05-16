#include "SimulationStepColorAdvection.h"

#include "Source/WSimSimulation/Simulation/Simulation.h"

SimulationStepColorAdvection::SimulationStepColorAdvection(Simulation &simulation)
    : SimulationStep(simulation, simulation.getSimulationSize(), simulation.getSimulationSizeWithBorder()),
      kernelColorAdvection(simulation.getKernelManager()["advection.cl"]["advect"]) {}

void SimulationStepColorAdvection::run(float deltaTimeSeconds) {
    auto &color = simulation.getColor();
    auto &velocity = simulation.getVelocity();
    auto &commandQueue = simulation.getCommandQueue();

    auto gws = simulation.getSimulationSizeWithBorder(); // TODO advection is done on border areas too
    auto velocityOffset = calculateBorderOffset(simulation.getSimulationSizeWithBorder(), gws, simulation.getPositionInGrid());
    OCL::setKernelArgMem(kernelColorAdvection, 0, color.getSource());                                    // inField
    OCL::setKernelArgMem(kernelColorAdvection, 1, velocity.getSource());                                 // inVelocity
    OCL::setKernelArgVec(kernelColorAdvection, 2, velocityOffset.x, velocityOffset.y, velocityOffset.z); // inVelocityOffset
    OCL::setKernelArgFlt(kernelColorAdvection, 3, deltaTimeSeconds);                                     // inDeltaTime
    OCL::setKernelArgFlt(kernelColorAdvection, 4, 1.f);                                                  // inDissipation
    OCL::setKernelArgMem(kernelColorAdvection, 5, color.getDestinationAndSwap());                        // outField
    OCL::enqueueKernel3D(commandQueue, kernelColorAdvection, gws);
}

void SimulationStepColorAdvection::stop() {}
