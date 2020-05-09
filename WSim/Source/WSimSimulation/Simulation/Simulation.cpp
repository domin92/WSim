#include "Simulation.h"

#include "Source/WSimSimulation/Simulation/SimulationStep.h"

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

void Simulation::stepSimulation(float deltaTime) {
    // Run SimulationSteps. They were created in reverse order, so we iterate in that order as well
    for (auto it = simulationSteps.rbegin(); it != simulationSteps.rend(); it++) {
        (*it)->run(deltaTime);
    }

    // Advect Color - including the border
    auto kernelAdvection = kernels["advection.cl"]["advection3f"];
    OCL::setKernelArgMem(kernelAdvection, 0, color.getSource());             // inField
    OCL::setKernelArgMem(kernelAdvection, 1, velocity.getSource());          // inVelocity
    OCL::setKernelArgMem(kernelAdvection, 2, obstacles);                     // inObstacles TODO
    OCL::setKernelArgVec(kernelAdvection, 3, 0.f, 0.f, 0.f);                 // inVelocityOffset
    OCL::setKernelArgFlt(kernelAdvection, 4, deltaTime);                     // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 5, 1.f);                           // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 6, color.getDestinationAndSwap()); // outField
    OCL::enqueueKernel3D(commandQueue, kernelAdvection, simulationSizeWithBorder);
}

size_t Simulation::getSubImagesCount2D() {
    return 1u;
}

Simulation::SubImageInfo Simulation::getSubImageInfo2D(size_t subImageIndex) {
    wsimErrorIf(subImageIndex != 0);
    SubImageInfo info;
    info.xOffset = 0;
    info.yOffset = 0;
    info.width = simulationSize.x;
    info.height = simulationSize.y;
    info.valid = true;
    return info;
}

void Simulation::getSubImage2D(size_t subImageIndex, void *data) {
    wsimErrorIf(subImageIndex != 0);
    OCL::Vec3 offset = borderOffset;
    offset.z = 0;
    OCL::Vec3 size = simulationSize;
    size.z = 1;
    OCL::enqueueReadImage3D(commandQueue, color.getSource(), CL_TRUE, offset, size, data);
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

    float value[4] = { 0,0,0,0 };
    value[static_cast<int>(dimension)] = (end == End::Higher) ? 1 : 1;

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
