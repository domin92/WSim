#include "SimulationStepVorticityConfinement.h"

#include "Source/WSimSimulation/Simulation/Simulation.h"

SimulationStepVorticityConfinement::SimulationStepVorticityConfinement(Simulation &simulation, Vec3 &outputVelocitySize)
    : SimulationStep(simulation, outputVelocitySize, increaseBorder(outputVelocitySize, simulation.getPositionInGrid(), 2)),
      vorticitySize(decreaseBorder(inputVelocitySize, simulation.getPositionInGrid(), 1, simulation.getSimulationSize())),
      kernelCalculateVorticity(simulation.getKernelManager()["vorticityConfinement.cl"]["calculateVorticity"]),
      kernelApplyVorticityConfinement(simulation.getKernelManager()["vorticityConfinement.cl"]["applyVorticityConfinement"]),
      vorticity(OCL::createReadWriteImage3D(simulation.getContext(), vorticitySize, vectorFieldFormat)) {}

void SimulationStepVorticityConfinement::run(float deltaTimeSeconds) {
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
