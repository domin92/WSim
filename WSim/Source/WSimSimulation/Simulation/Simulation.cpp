#include "Simulation.h"

#include "Source/WSimSimulation/Simulation/Step/SimulationStepAdvection.h"
#include "Source/WSimSimulation/Simulation/Step/SimulationStepVorticityConfinement.h"
#include "Source/WSimSimulation/Simulation/Step/SimulationStepVorticityPressure.h"

Simulation::Simulation(size_t platformIndex, size_t deviceIndex, OCL::Vec3 simulationSize)
    : Simulation(platformIndex, deviceIndex, simulationSize, 0, PositionInGrid{OCL::Vec3{0, 0, 0}, OCL::Vec3{1, 1, 1}}) {}

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
      obstacles(OCL::createReadWriteImage3D(context, simulationSize, vectorFieldFormat)),
      kernels(device, context),
      kernelFillVelocity(kernels["fill.cl"]["fillVelocity"]),
      kernelFillColor(kernels["fill.cl"]["fillColor"]),
      kernelAddVelocity(kernels["addVelocity.cl"]["addVelocity"]) {

    // Create SimulationSteps in reverse order
    OCL::Vec3 currentSimulationSize = simulationSize;
    simulationSteps.emplace_back(new SimulationStepPressure(*this, 5, currentSimulationSize));
    simulationSteps.emplace_back(new SimulationStepVorticityConfinement(*this, currentSimulationSize));
    simulationSteps.emplace_back(new SimulationStepAdvection(*this, currentSimulationSize));

    OCL::enqueueZeroImage3D(commandQueue, obstacles, simulationSize);
    reset();
}

void Simulation::stepSimulation(float deltaTimeSeconds) {
    // Run SimulationSteps. They were created in reverse order, so we iterate in that order as well
    for (auto it = simulationSteps.rbegin(); it != simulationSteps.rend(); it++) {
        (*it)->run(deltaTimeSeconds);
    }

    // Advect Color - including the border
    auto kernelAdvection = kernels["advection.cl"]["advect"];
    OCL::setKernelArgMem(kernelAdvection, 0, color.getSource());             // inField
    OCL::setKernelArgMem(kernelAdvection, 1, velocity.getSource());          // inVelocity
    OCL::setKernelArgVec(kernelAdvection, 2, 0.f, 0.f, 0.f);                 // inVelocityOffset
    OCL::setKernelArgFlt(kernelAdvection, 3, deltaTimeSeconds);              // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 4, 1.f);                           // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 5, color.getDestinationAndSwap()); // outField
    OCL::enqueueKernel3D(commandQueue, kernelAdvection, simulationSizeWithBorder);
}

void Simulation::applyForce(float positionX, float positionY, float changeX, float changeY, float radius) {
    const float coefficient = 0.01f; // arbitrarily set
    changeX *= simulationSize.x * coefficient;
    changeY *= simulationSize.y * coefficient;

    OCL::setKernelArgMem(kernelAddVelocity, 0, velocity.getSource());             // inVelocity
    OCL::setKernelArgVec(kernelAddVelocity, 1, positionX, positionY, 0);          // inCenter
    OCL::setKernelArgVec(kernelAddVelocity, 2, changeX, changeY, 0);              // inVelocityChange
    OCL::setKernelArgFlt(kernelAddVelocity, 3, radius);                           // inRadius
    OCL::setKernelArgMem(kernelAddVelocity, 4, velocity.getDestinationAndSwap()); // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelAddVelocity, simulationSizeWithBorder);
}

void Simulation::stop() {
    OCL::enqueueZeroImage3D(commandQueue, velocity.getSource(), simulationSizeWithBorder);
    OCL::enqueueZeroImage3D(commandQueue, velocity.getDestination(), simulationSizeWithBorder);
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

void Simulation::addObstacleWall(Dim dimension, End end) {
    OCL::Vec3 offset = {0, 0, 0};
    if (end == End::Higher) {
        selectDimension(offset, dimension) += (selectDimension(simulationSize, dimension) - 1);
    }

    OCL::Vec3 size = simulationSize;
    selectDimension(size, dimension) = 1;

    float value[4] = {0, 0, 0, 0};
    value[static_cast<int>(dimension)] = (end == End::Higher) ? 1.f : 1.f; // TODO

    OCL::enqueueFillImage3D(commandQueue, obstacles, value, offset, size);

    auto b = std::make_unique<char[]>(simulationSize.getRequiredBufferSize(16));
    OCL::enqueueReadImage3D(commandQueue, obstacles, CL_BLOCKING, simulationSize, b.get());
    int a = 0;
}

void Simulation::addObstacleAllWalls() {
    addObstacleWall(Dim::X, End::Lower);
    addObstacleWall(Dim::X, End::Higher);
    addObstacleWall(Dim::Y, End::Lower);
    addObstacleWall(Dim::Y, End::Higher);
    if (simulationSize.z >= 3) {
        addObstacleWall(Dim::Z, End::Lower);
        addObstacleWall(Dim::Z, End::Higher);
    }
}
