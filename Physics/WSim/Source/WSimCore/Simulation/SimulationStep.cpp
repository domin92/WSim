#include "SimulationStep.h"

#include "Source/WSimCore/Simulation/Simulation.h"
#include "Source/WSimCore/Utils/Error.h"

SimulationStep::SimulationStep(Simulation &simulation, OCL::Vec3 &outputVelocitySize, OCL::Vec3 inputVelocitySize)
    : simulation(simulation),
      outputVelocitySize(outputVelocitySize),
      inputVelocitySize(inputVelocitySize) {
    // Simulation size can't be enlarged by the simulation step
    wsimErrorUnless(outputVelocitySize <= inputVelocitySize);

    // Input size of this step, will be use as an output size of previous steps, which will be constructed later
    // (steps are constructed from last to first)
    outputVelocitySize = inputVelocitySize;
}

SimulationStepAdvection::SimulationStepAdvection(Simulation &simulation, OCL::Vec3 &outputVelocitySize)
    : SimulationStep(simulation, outputVelocitySize, simulation.getSimulationSizeWithBorder()),
      kernelAdvection(simulation.getKernelManager()["advection.cl"]["advection3f"]) {}

void SimulationStepAdvection::run(float deltaTime) {
    auto &velocity = simulation.getVelocity();
    auto &commandQueue = simulation.getCommandQueue();

    auto gws = outputVelocitySize;
    auto velocityOffset = calculateBorderOffset(simulation.getSimulationSizeWithBorder(), gws, simulation.getPositionInGrid());
    OCL::setKernelArgMem(kernelAdvection, 0, velocity.getSource());                                 // inField
    OCL::setKernelArgMem(kernelAdvection, 1, velocity.getSource());                                 // inVelocity
    OCL::setKernelArgVec(kernelAdvection, 2, velocityOffset.x, velocityOffset.y, velocityOffset.z); // inVelocityOffset
    OCL::setKernelArgFlt(kernelAdvection, 3, deltaTime);                                            // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 4, 1.f);                                                  // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 5, velocity.getDestinationAndSwap());                     // outField
    OCL::enqueueKernel3D(commandQueue, kernelAdvection, outputVelocitySize);
}

void SimulationStepAdvection::stop() {}

SimulationStepVorticityConfinement::SimulationStepVorticityConfinement(Simulation &simulation, OCL::Vec3 &outputVelocitySize)
    : SimulationStep(simulation, outputVelocitySize, increaseBorder(outputVelocitySize, simulation.getPositionInGrid(), 2)),
      vorticitySize(decreaseBorder(inputVelocitySize, simulation.getPositionInGrid(), 1, simulation.getSimulationSize())),
      kernelCalculateVorticity(simulation.getKernelManager()["vorticityConfinement.cl"]["calculateVorticity"]),
      kernelApplyVorticityConfinement(simulation.getKernelManager()["vorticityConfinement.cl"]["applyVorticityConfinement"]),
      vorticity(OCL::createReadWriteImage3D(simulation.getContext(), vorticitySize, vectorFieldFormat)) {}

void SimulationStepVorticityConfinement::run(float deltaTime) {
    auto &velocity = simulation.getVelocity();
    auto &commandQueue = simulation.getCommandQueue();

    // Calculate vorticity
    auto gws = vorticitySize;
    auto velocityOffset = calculateBorderOffset(simulation.getSimulationSizeWithBorder(), gws, simulation.getPositionInGrid());
    OCL::setKernelArgMem(kernelCalculateVorticity, 0, velocity.getSource());                                 // inVelocity
    OCL::setKernelArgVec(kernelCalculateVorticity, 1, velocityOffset.x, velocityOffset.y, velocityOffset.z); // inVelocityOffset
    OCL::setKernelArgMem(kernelCalculateVorticity, 2, vorticity);                                            // outVorticity
    OCL::enqueueKernel3D(commandQueue, kernelCalculateVorticity, gws);

    // Decrease size, as we'll be computing gradient of vorticity, which of size gws.
    gws = decreaseBorder(gws, simulation.getPositionInGrid(), 1, simulation.getSimulationSize());
    velocityOffset = calculateBorderOffset(simulation.getSimulationSizeWithBorder(), gws, simulation.getPositionInGrid());
    const auto vorticityOffset = calculateBorderOffset(vorticitySize, gws, simulation.getPositionInGrid());

    // Apply vorticity confinement
    OCL::setKernelArgMem(kernelApplyVorticityConfinement, 0, velocity.getSource());                                    // inVelocity
    OCL::setKernelArgMem(kernelApplyVorticityConfinement, 1, vorticity);                                               // inVorticity
    OCL::setKernelArgVec(kernelApplyVorticityConfinement, 2, velocityOffset.x, velocityOffset.y, velocityOffset.z);    // inVelocityOffset
    OCL::setKernelArgVec(kernelApplyVorticityConfinement, 3, vorticityOffset.x, vorticityOffset.y, vorticityOffset.z); // inVorticityOffset
    OCL::setKernelArgFlt(kernelApplyVorticityConfinement, 4, 10.f);                                                    // inVorticityStrength
    OCL::setKernelArgMem(kernelApplyVorticityConfinement, 5, velocity.getDestinationAndSwap());                        // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelApplyVorticityConfinement, gws);
}

void SimulationStepVorticityConfinement::stop() {
    OCL::enqueueZeroImage3D(simulation.getCommandQueue(), vorticity, vorticitySize);
}

SimulationStepPressure::SimulationStepPressure(Simulation &simulation, size_t jacobiIterations, OCL::Vec3 &outputVelocitySize)
    : SimulationStep(simulation, outputVelocitySize, increaseBorder(outputVelocitySize, simulation.getPositionInGrid(), static_cast<int>(jacobiIterations + 1))),
      jacobiIterations(jacobiIterations),
      divergenceSize(decreaseBorder(inputVelocitySize, simulation.getPositionInGrid(), 1, simulation.getSimulationSize())),
      kernelDivergence(simulation.getKernelManager()["pressure.cl"]["calculateDivergence"]),
      kernelPressureJacobi(simulation.getKernelManager()["pressure.cl"]["calculatePressureWithJacobiIteration"]),
      kernelProjectVelocityToDivergenceFree(simulation.getKernelManager()["pressure.cl"]["projectVelocityToDivergenceFree"]),
      divergence(OCL::createReadWriteImage3D(simulation.getContext(), divergenceSize, scalarFieldFormat)),
      pressure(simulation.getContext(), divergenceSize, scalarFieldFormat) {}

void SimulationStepPressure::run(float deltaTime) {
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
}
