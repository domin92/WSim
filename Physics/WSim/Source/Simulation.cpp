#include "Simulation.h"

Simulation::Simulation(OCL::Vec3 imageSize)
    : imageSize(imageSize),
      platform(OCL::createPlatform()),
      device(OCL::createDevice(platform)),
      context(OCL::createContext(platform, device)),
      commandQueue(OCL::createCommandQueue(context, device)),
      velocity(context, imageSize, vectorFieldFormat),
      divergence(context, imageSize, scalarFieldFormat),
      pressure(context, imageSize, scalarFieldFormat),
      color(context, imageSize, vectorFieldFormat) {

    // Load kernels
    this->programs.push_back(OCL::createProgramFromFile(device, context, "fill.cl", true));
    this->kernelFillVelocity = OCL::createKernel(programs.back(), "fillVelocity");
    this->kernelFillColor = OCL::createKernel(programs.back(), "fillColor");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "advection.cl", true));
    this->kernelAdvection = OCL::createKernel(programs.back(), "advection3f");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "pressure/divergence.cl", true));
    this->kernelDivergence = OCL::createKernel(programs.back(), "calculate_divergence");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "pressure/jacobi.cl", true));
    this->kernelPressureJacobi = OCL::createKernel(programs.back(), "solve_jacobi_iteration");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "pressure/apply_pressure.cl", true));
    this->kernelApplyPressure = OCL::createKernel(programs.back(), "apply_pressure");
    this->programs.push_back(OCL::createProgramFromFile(device, context, "addVelocity.cl", true));
    this->kernelAddVelocity = OCL::createKernel(programs.back(), "addVelocity");

    // Fill velocity buffer
    OCL::setKernelArgFlt(kernelFillVelocity, 0, imageSize.x);               // inImageSize
    OCL::setKernelArgMem(kernelFillVelocity, 1, velocity.getDestination()); // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelFillVelocity, imageSize);
    velocity.swap();

    // Fill color buffer
    OCL::setKernelArgFlt(kernelFillColor, 0, imageSize.x);            // inImageSize
    OCL::setKernelArgMem(kernelFillColor, 1, color.getDestination()); // outColor
    OCL::enqueueKernel3D(commandQueue, kernelFillColor, imageSize);
    color.swap();
}

void Simulation::stepSimulation(float deltaTime) {
    // Advect velocity
    OCL::setKernelArgMem(kernelAdvection, 0, velocity.getSource());      // inField
    OCL::setKernelArgMem(kernelAdvection, 1, velocity.getSource());      // inVelocity
    OCL::setKernelArgFlt(kernelAdvection, 2, deltaTime);                 // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 3, 1.f);                       // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 4, velocity.getDestination()); // outField
    OCL::enqueueKernel3D(commandQueue, kernelAdvection, imageSize);
    velocity.swap();

    // Calculate divergence
    OCL::setKernelArgMem(kernelDivergence, 0, velocity.getSource());        // inVelocity
    OCL::setKernelArgMem(kernelDivergence, 1, divergence.getDestination()); // outDivergence
    OCL::enqueueKernel3D(commandQueue, kernelDivergence, imageSize);
    divergence.swap();

    // Calculate pressure
    OCL::setKernelArgMem(kernelPressureJacobi, 0, divergence.getSource());        // inDivergence
    for (int i = 0; i < 10; i++) {                                                //
        OCL::setKernelArgMem(kernelPressureJacobi, 1, pressure.getSource());      // inPressure
        OCL::setKernelArgMem(kernelPressureJacobi, 2, pressure.getDestination()); // outPressure
        OCL::enqueueKernel3D(commandQueue, kernelPressureJacobi, imageSize);
        pressure.swap();
    }

    // Apply pressure
    OCL::setKernelArgMem(kernelApplyPressure, 0, velocity.getSource());      // inVelocity
    OCL::setKernelArgMem(kernelApplyPressure, 1, pressure.getSource());      // inPressure
    OCL::setKernelArgMem(kernelApplyPressure, 2, velocity.getDestination()); // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelApplyPressure, imageSize);
    velocity.swap();

    // Advect Color
    OCL::setKernelArgMem(kernelAdvection, 0, color.getSource());      // inField
    OCL::setKernelArgMem(kernelAdvection, 1, velocity.getSource());   // inVelocity
    OCL::setKernelArgFlt(kernelAdvection, 2, deltaTime);              // inDeltaTime
    OCL::setKernelArgFlt(kernelAdvection, 3, 1.f);                    // inDissipation
    OCL::setKernelArgMem(kernelAdvection, 4, color.getDestination()); // outField
    OCL::enqueueKernel3D(commandQueue, kernelAdvection, imageSize);
    color.swap();
}

void Simulation::applyForce(float positionX, float positionY, float changeX, float changeY, float radius) {
    const float coefficient = 0.01f; // arbitrarily set
    changeX *= imageSize.x * coefficient;
    changeY *= imageSize.y * coefficient;

    OCL::setKernelArgMem(kernelAddVelocity, 0, velocity.getSource());    // inVelocity
    OCL::setKernelArgVec(kernelAddVelocity, 1, positionX, positionY, 0); // inCenter
    OCL::setKernelArgVec(kernelAddVelocity, 2, changeX, changeY, 0);     // inVelocityChange
    OCL::setKernelArgFlt(kernelAddVelocity, 3, radius);                  // inRadius
    OCL::setKernelArgMem(kernelAddVelocity, 4, velocity.getDestination());
    OCL::enqueueKernel3D(commandQueue, kernelAddVelocity, imageSize);
    velocity.swap();
}

void Simulation::stop() {
    OCL::enqueueZeroImage3D(commandQueue, velocity.getSource(), imageSize);
}
