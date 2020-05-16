#include "Simulation.h"

#include "Source/WSimSimulation/Simulation/Step/SimulationStepAdvection.h"
#include "Source/WSimSimulation/Simulation/Step/SimulationStepColorAdvection.h"
#include "Source/WSimSimulation/Simulation/Step/SimulationStepVorticityConfinement.h"
#include "Source/WSimSimulation/Simulation/Step/SimulationStepVorticityPressure.h"

Simulation::Simulation(size_t platformIndex, size_t deviceIndex, Vec3 simulationSize)
    : Simulation(platformIndex, deviceIndex, simulationSize, 0, PositionInGrid{Vec3{0, 0, 0}, Vec3{1, 1, 1}}) {}

Simulation::Simulation(size_t platformIndex, size_t deviceIndex, Vec3 simulationSize, size_t borderWidth, PositionInGrid positionInGrid)
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
    Vec3 currentSimulationSize = simulationSize;
    simulationSteps.emplace_back(new SimulationStepColorAdvection(*this));
    simulationSteps.emplace_back(new SimulationStepPressure(*this, 5, currentSimulationSize));
    //simulationSteps.emplace_back(new SimulationStepVorticityConfinement(*this, currentSimulationSize)); // Doesn't work in 3D
    simulationSteps.emplace_back(new SimulationStepAdvection(*this, currentSimulationSize));

    OCL::enqueueZeroImage3D(commandQueue, obstacles, simulationSize);
    reset();
}

void Simulation::stepSimulation(float deltaTimeSeconds) {
    // Run SimulationSteps. They were created in reverse order, so we iterate in that order as well
    for (auto it = simulationSteps.rbegin(); it != simulationSteps.rend(); it++) {
        (*it)->run(deltaTimeSeconds);
    }
}

void Simulation::applyForce(float positionX, float positionY, float changeX, float changeY, float radius) {
    applyForce(FloatVec3{positionX, positionY, 0}, FloatVec3{changeX, changeY, 0}, radius);
}

void Simulation::applyForce(FloatVec3 position, FloatVec3 change, float radius) {
    const float coefficient = 0.01f; // arbitrarily set
    change.x *= simulationSize.x * coefficient;
    change.y *= simulationSize.y * coefficient;
    change.z *= simulationSize.z * coefficient;

    OCL::setKernelArgMem(kernelAddVelocity, 0, velocity.getSource());                           // inVelocity
    OCL::setKernelArgVec(kernelAddVelocity, 1, borderOffset.x, borderOffset.y, borderOffset.z); // inVelocityOffset
    OCL::setKernelArgVec(kernelAddVelocity, 2, position.x, position.y, position.z);             // inCenter
    OCL::setKernelArgVec(kernelAddVelocity, 3, change.x, change.y, change.z);                   // inVelocityChange
    OCL::setKernelArgFlt(kernelAddVelocity, 4, radius);                                         // inRadius
    OCL::setKernelArgMem(kernelAddVelocity, 5, velocity.getDestinationAndSwap());               // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelAddVelocity, simulationSize);
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
    Vec3 offset = {0, 0, 0};
    if (end == End::Higher) {
        selectDimension(offset, dimension) += (selectDimension(simulationSize, dimension) - 1);
    }

    Vec3 size = simulationSize;
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
