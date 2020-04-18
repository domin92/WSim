#include "Simulation.h"

Simulation::Simulation(size_t platformIndex, size_t deviceIndex, OCL::Vec3 simulationSize, size_t borderWidth, PositionInGrid positionInGrid)
    : positionInGrid(positionInGrid),
      borderOffset(calculateBorderOffset(positionInGrid, borderWidth)),
      simulationSize(simulationSize),
      simulationSizeWithBorder(calculateSimulationSizeWithBorder(simulationSize, positionInGrid, borderWidth)),
      platform(OCL::createPlatform(platformIndex)),
      device(OCL::createDevice(platform, CL_DEVICE_TYPE_GPU, deviceIndex)),
      context(OCL::createContext(platform, device)),
      commandQueue(OCL::createCommandQueue(context, device)),
      velocity(context, simulationSizeWithBorder, vectorFieldFormat),
      color(context, simulationSizeWithBorder, vectorFieldFormat),
      divergence(context, simulationSize, scalarFieldFormat),
      pressure(context, simulationSize, scalarFieldFormat),
      vorticity(context, simulationSize, vectorFieldFormat) {

    // Load kernels
    this->programs.push_back(OCL::createProgramFromFile(device, context, "fill.cl", true));
    this->kernelFillVelocity = OCL::createKernel(programs.back(), "fillVelocity");
    this->kernelFillColor = OCL::createKernel(programs.back(), "fillColor");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "advection.cl", true));
    this->kernelAdvection = OCL::createKernel(programs.back(), "advection3f");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "vorticityConfinement.cl", true));
    this->kernelCalculateVorticity = OCL::createKernel(programs.back(), "calculateVorticity");
    this->kernelApplyVorticityConfinement = OCL::createKernel(programs.back(), "applyVorticityConfinement");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "pressure.cl", true));
    this->kernelDivergence = OCL::createKernel(programs.back(), "calculateDivergence");
    this->kernelPressureJacobi = OCL::createKernel(programs.back(), "calculatePressureWithJacobiIteration");
    this->kernelProjectVelocityToDivergenceFree = OCL::createKernel(programs.back(), "projectVelocityToDivergenceFree");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "addVelocity.cl", true));
    this->kernelAddVelocity = OCL::createKernel(programs.back(), "addVelocity");
    reset();
}

void Simulation::stepSimulation(float deltaTime) {
    // Advect velocity - including the border, because it will be sampled from later
    OCL::setKernelArgMem(kernelAdvection, 0, velocity.getSource());             // inField
    OCL::setKernelArgMem(kernelAdvection, 1, velocity.getSource());             // inVelocity
    OCL::setKernelArgFlt(kernelAdvection, 2, deltaTime);                        // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 3, 1.f);                              // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 4, velocity.getDestinationAndSwap()); // outField
    OCL::enqueueKernel3D(commandQueue, kernelAdvection, simulationSizeWithBorder);

    // Calculate vorticity
    OCL::setKernelArgMem(kernelCalculateVorticity, 0, velocity.getSource());                           // inVelocity
    OCL::setKernelArgVec(kernelCalculateVorticity, 1, borderOffset.x, borderOffset.y, borderOffset.z); // inVelocityOffset
    OCL::setKernelArgMem(kernelCalculateVorticity, 2, vorticity.getDestinationAndSwap());              // outVorticity
    OCL::enqueueKernel3D(commandQueue, kernelCalculateVorticity, simulationSize);                      // TODO: next kernel calculates vorticity gradient. Calculate this with border=1?

    // Apply vorticity confinement
    // TODO problem with border
    OCL::setKernelArgMem(kernelApplyVorticityConfinement, 0, velocity.getSource());                           // inVelocity
    OCL::setKernelArgMem(kernelApplyVorticityConfinement, 1, vorticity.getSource());                          // inVorticity
    OCL::setKernelArgVec(kernelApplyVorticityConfinement, 2, borderOffset.x, borderOffset.y, borderOffset.z); // inVelocityOffset
    OCL::setKernelArgFlt(kernelApplyVorticityConfinement, 3, 10.f);                                           // inVelocityStrength
    OCL::setKernelArgMem(kernelApplyVorticityConfinement, 4, velocity.getDestinationAndSwap());               // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelApplyVorticityConfinement, simulationSizeWithBorder);

    // Calculate divergence
    OCL::setKernelArgMem(kernelDivergence, 0, velocity.getSource());                           // inVelocity
    OCL::setKernelArgVec(kernelDivergence, 1, borderOffset.x, borderOffset.y, borderOffset.z); // inVelocityOffset
    OCL::setKernelArgMem(kernelDivergence, 2, divergence.getDestinationAndSwap());             // outDivergence
    OCL::enqueueKernel3D(commandQueue, kernelDivergence, simulationSize);

    // Calculate pressure
    OCL::setKernelArgMem(kernelPressureJacobi, 0, divergence.getSource());               // inDivergence
    for (int i = 0; i < 10; i++) {                                                       //
        OCL::setKernelArgMem(kernelPressureJacobi, 1, pressure.getSource());             // inPressure
        OCL::setKernelArgMem(kernelPressureJacobi, 2, pressure.getDestinationAndSwap()); // outPressure
        OCL::enqueueKernel3D(commandQueue, kernelPressureJacobi, simulationSize);
    }

    // Apply pressure
    OCL::setKernelArgMem(kernelProjectVelocityToDivergenceFree, 0, velocity.getSource());                           // inVelocity
    OCL::setKernelArgMem(kernelProjectVelocityToDivergenceFree, 1, pressure.getSource());                           // inPressure
    OCL::setKernelArgVec(kernelProjectVelocityToDivergenceFree, 2, borderOffset.x, borderOffset.y, borderOffset.z); // inVelocityOffset
    OCL::setKernelArgMem(kernelProjectVelocityToDivergenceFree, 3, velocity.getDestinationAndSwap());               // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelProjectVelocityToDivergenceFree, simulationSize);

    // Advect Color - including the border
    OCL::setKernelArgMem(kernelAdvection, 0, color.getSource());             // inField
    OCL::setKernelArgMem(kernelAdvection, 1, velocity.getSource());          // inVelocity
    OCL::setKernelArgFlt(kernelAdvection, 2, deltaTime);                     // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 3, 1.f);                           // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 4, color.getDestinationAndSwap()); // outField
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
    OCL::enqueueZeroImage3D(commandQueue, divergence.getDestinationAndSwap(), simulationSize);
    OCL::enqueueZeroImage3D(commandQueue, pressure.getDestinationAndSwap(), simulationSize);
    OCL::enqueueZeroImage3D(commandQueue, vorticity.getDestinationAndSwap(), simulationSize);
}

void Simulation::reset() {
    OCL::setKernelArgFlt(kernelFillVelocity, 0, static_cast<float>(simulationSize.x)); // inImageSize
    OCL::setKernelArgMem(kernelFillVelocity, 1, velocity.getDestinationAndSwap());     // outVelocity

    OCL::enqueueKernel3D(commandQueue, kernelFillVelocity, simulationSizeWithBorder);
    OCL::setKernelArgVec(kernelFillColor, 0, simulationSize.x, simulationSize.y, simulationSize.z); // inImageSize
    OCL::setKernelArgVec(kernelFillColor, 1, borderOffset.x, borderOffset.y, borderOffset.z);       // inOffset
    OCL::setKernelArgMem(kernelFillColor, 2, color.getDestinationAndSwap());                        // outColor
    OCL::enqueueKernel3D(commandQueue, kernelFillColor, simulationSizeWithBorder);

    OCL::enqueueZeroImage3D(commandQueue, divergence.getDestinationAndSwap(), simulationSize);
    OCL::enqueueZeroImage3D(commandQueue, pressure.getDestinationAndSwap(), simulationSize);
    OCL::enqueueZeroImage3D(commandQueue, vorticity.getDestinationAndSwap(), simulationSize);
}

OCL::Vec3 Simulation::calculateSimulationSizeWithBorder(OCL::Vec3 simulationSize, PositionInGrid positionInGrid, size_t borderWidth) {

    const auto borderX = 2 - static_cast<int>(positionInGrid.edgeL) - static_cast<int>(positionInGrid.edgeR);
    const auto borderY = 2 - static_cast<int>(positionInGrid.edgeU) - static_cast<int>(positionInGrid.edgeD);
    const auto borderZ = 2 - static_cast<int>(positionInGrid.edgeF) - static_cast<int>(positionInGrid.edgeB);
    OCL::Vec3 result = {
        simulationSize.x + borderX * borderWidth,
        simulationSize.y + borderY * borderWidth,
        simulationSize.z + borderZ * borderWidth};
    return result;
}

OCL::Vec3 Simulation::calculateBorderOffset(PositionInGrid positionInGrid, size_t borderWidth) {
    OCL::Vec3 result{};
    if (!positionInGrid.edgeL) {
        result.x = borderWidth;
    }
    if (!positionInGrid.edgeD) {
        result.y = borderWidth;
    }
    if (!positionInGrid.edgeF) {
        result.z = borderWidth;
    }
    return result;
}
