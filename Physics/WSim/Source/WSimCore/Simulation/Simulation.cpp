#include "Simulation.h"

#include "Source/WSimCore/Simulation/SimulationStep.h"

Simulation::Simulation(size_t platformIndex, size_t deviceIndex, OCL::Vec3 simulationSize, size_t borderWidth, PositionInGrid positionInGrid)
    : positionInGrid(positionInGrid),
      simulationSize(simulationSize),
      simulationSizeWithBorder(increaseBorder(simulationSize, positionInGrid, static_cast<int>(borderWidth))),
      borderOffset(calculateBorderOffset(simulationSizeWithBorder, simulationSize, positionInGrid)),
      platform(OCL::createPlatform(platformIndex)),
      device(OCL::createDevice(platform, CL_DEVICE_TYPE_GPU, deviceIndex)),
      context(OCL::createContext(platform, device)),
      commandQueue(OCL::createCommandQueue(context, device)),
      velocity(context, simulationSizeWithBorder, vectorFieldFormat),
      color(context, simulationSizeWithBorder, vectorFieldFormat),
      kernels(device, context),
      kernelFillVelocity(kernels["fill.cl"]["fillVelocity"]),
      kernelFillColor(kernels["fill.cl"]["fillColor"]),
      kernelAddVelocity(kernels["addVelocity.cl"]["addVelocity"]) {

    // Create SimulationSteps in reverse order
    OCL::Vec3 currentSimulationSize = simulationSize;
    simulationSteps.emplace_back(new SimulationStepPressure(*this, 5, currentSimulationSize));
    simulationSteps.emplace_back(new SimulationStepVorticityConfinement(*this, currentSimulationSize));
    simulationSteps.emplace_back(new SimulationStepAdvection(*this, currentSimulationSize));

    reset();
}

void Simulation::stepSimulation(float deltaTime) {
    // Run SimulationSteps. They were created in reverse order, so we iterate in that order as well
    for (auto it = simulationSteps.rbegin(); it != simulationSteps.rend(); it++) {
        (*it)->run(deltaTime);
    }

    // Advect Color - including the border
    auto kernelAdvection = kernels["advection.cl"]["advection3f"];
    OCL::setKernelArgMem(kernelAdvection, 0, color.getSource());             // inField
    OCL::setKernelArgMem(kernelAdvection, 1, velocity.getSource());          // inVelocity
    OCL::setKernelArgVec(kernelAdvection, 2, 0.f, 0.f, 0.f);                 // inVelocityOffset
    OCL::setKernelArgFlt(kernelAdvection, 3, deltaTime);                     // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 4, 1.f);                           // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 5, color.getDestinationAndSwap()); // outField
    OCL::enqueueKernel3D(commandQueue, kernelAdvection, simulationSizeWithBorder);
}

void Simulation::applyForce(float positionX, float positionY, float changeX, float changeY, float radius) {
    const float coefficient = 0.01f; // arbitrarily set
    changeX *= simulationSize.x * coefficient;
    changeY *= simulationSize.y * coefficient;

    OCL::setKernelArgMem(kernelAddVelocity, 0, velocity.getSource());    // inVelocity
    OCL::setKernelArgVec(kernelAddVelocity, 1, positionX, positionY, 0); // inCenter
    OCL::setKernelArgVec(kernelAddVelocity, 2, changeX, changeY, 0);     // inVelocityChange
    OCL::setKernelArgFlt(kernelAddVelocity, 3, radius);                  // inRadius
    OCL::setKernelArgMem(kernelAddVelocity, 4, velocity.getDestinationAndSwap());
    OCL::enqueueKernel3D(commandQueue, kernelAddVelocity, simulationSizeWithBorder);
}

void Simulation::stop() {
    OCL::enqueueZeroImage3D(commandQueue, velocity.getSource(), simulationSizeWithBorder);
    for (auto it = simulationSteps.rbegin(); it != simulationSteps.rend(); it++) {
        (*it)->stop();
    }
}

void Simulation::reset() {
    OCL::setKernelArgFlt(kernelFillVelocity, 0, static_cast<float>(simulationSize.x)); // inImageSize
    OCL::setKernelArgMem(kernelFillVelocity, 1, velocity.getDestinationAndSwap());     // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelFillVelocity, simulationSizeWithBorder);

    OCL::setKernelArgVec(kernelFillColor, 0, simulationSize.x, simulationSize.y, simulationSize.z); // inImageSize
    OCL::setKernelArgVec(kernelFillColor, 1, borderOffset.x, borderOffset.y, borderOffset.z);       // inOffset
    OCL::setKernelArgMem(kernelFillColor, 2, color.getDestinationAndSwap());                        // outColor
    OCL::enqueueKernel3D(commandQueue, kernelFillColor, simulationSizeWithBorder);

    for (auto it = simulationSteps.rbegin(); it != simulationSteps.rend(); it++) {
        (*it)->stop();
    }
}
