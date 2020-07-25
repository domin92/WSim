#include "SimulationStepVorticityPressure.hpp"

#include "Source/WSimSimulation/Simulation/Simulation.hpp"

SimulationStepPressure::SimulationStepPressure(Simulation &simulation, size_t jacobiIterations, Vec3 &outputVelocitySize)
    : SimulationStep(simulation, outputVelocitySize, increaseBorder(outputVelocitySize, simulation.getPositionInGrid(), static_cast<int>(jacobiIterations + 1))),
      jacobiIterations(jacobiIterations),
      divergenceSize(decreaseBorder(inputVelocitySize, simulation.getPositionInGrid(), 1, simulation.getSimulationSize())),
      kernelDivergence(simulation.getKernelManager()["pressure.cl"]["calculateDivergence"]),
      kernelPressureJacobi(simulation.getKernelManager()["pressure.cl"]["calculatePressureWithJacobiIteration"]),
      kernelProjectVelocityToDivergenceFree(simulation.getKernelManager()["pressure.cl"]["projectVelocityToDivergenceFree"]),
      divergence(OCL::createReadWriteImage3D(simulation.getContext(), divergenceSize, scalarFieldFormat)),
      pressure(simulation.getContext(), divergenceSize, scalarFieldFormat) {}

void SimulationStepPressure::run(float deltaTimeSeconds) {
    auto &velocity = simulation.getVelocity();
    auto &commandQueue = simulation.getCommandQueue();

    // Calculate divergence
    auto gws = divergenceSize;
    auto velocityOffset = calculateBorderOffset(simulation.getSimulationSizeWithBorder(), gws, simulation.getPositionInGrid());
    OCL::setKernelArgMem(kernelDivergence, 0, velocity.getSource());                                 // inVelocity
    OCL::setKernelArgVec(kernelDivergence, 1, velocityOffset.x, velocityOffset.y, velocityOffset.z); // inVelocityOffset
    OCL::setKernelArgMem(kernelDivergence, 2, divergence);                                           // outDivergence
    OCL::enqueueKernel3D(commandQueue, kernelDivergence, gws);

    // Calculate pressure
    OCL::setKernelArgMem(kernelPressureJacobi, 0, divergence); // inDivergence
    for (int i = 0; i < jacobiIterations; i++) {
        const auto pressureOffset = calculateBorderOffset(divergenceSize, gws, simulation.getPositionInGrid());
        OCL::setKernelArgMem(kernelPressureJacobi, 1, pressure.getSource());                                 // inPressure
        OCL::setKernelArgVec(kernelPressureJacobi, 2, pressureOffset.x, pressureOffset.y, pressureOffset.z); // inPressureOffset
        OCL::setKernelArgMem(kernelPressureJacobi, 3, pressure.getDestinationAndSwap());                     // outPressure
        OCL::enqueueKernel3D(commandQueue, kernelPressureJacobi, gws);

        // With each iteration we calculate smaller pressure field, because we sample one unit around each voxel
        gws = decreaseBorder(gws, simulation.getPositionInGrid(), 1, simulation.getSimulationSize());
    }

    // This should always be the last step, and it's output should not contain any borders, as that would be wasteful
    wsimErrorUnless(gws == simulation.getSimulationSize());

    // GWS is now smaller so we need a new offset
    velocityOffset = calculateBorderOffset(simulation.getSimulationSizeWithBorder(), gws, simulation.getPositionInGrid());
    const auto pressureOffset = calculateBorderOffset(divergenceSize, gws, simulation.getPositionInGrid());

    // Apply pressure
    OCL::setKernelArgMem(kernelProjectVelocityToDivergenceFree, 0, velocity.getSource());                                 // inVelocity
    OCL::setKernelArgMem(kernelProjectVelocityToDivergenceFree, 1, pressure.getSource());                                 // inPressure
    OCL::setKernelArgVec(kernelProjectVelocityToDivergenceFree, 2, velocityOffset.x, velocityOffset.y, velocityOffset.z); // inVelocityOffset
    OCL::setKernelArgVec(kernelProjectVelocityToDivergenceFree, 3, pressureOffset.x, pressureOffset.y, pressureOffset.z); // inPressureOffset
    OCL::setKernelArgMem(kernelProjectVelocityToDivergenceFree, 4, velocity.getDestinationAndSwap());                     // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelProjectVelocityToDivergenceFree, gws);
}

void SimulationStepPressure::stop() {
    OCL::enqueueZeroImage3D(simulation.getCommandQueue(), divergence, divergenceSize);
    OCL::enqueueZeroImage3D(simulation.getCommandQueue(), pressure.getDestination(), divergenceSize);
    OCL::enqueueZeroImage3D(simulation.getCommandQueue(), pressure.getSource(), divergenceSize);
}
