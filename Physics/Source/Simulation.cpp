#include "Simulation.h"

Simulation::Simulation(OCL::Vec3 imageSize)
    : imageSize(imageSize),
      platform(OCL::createPlatform()),
      device(OCL::createDevice(platform)),
      context(OCL::createContext(platform, device)),
      commandQueue(OCL::createCommandQueue(context, device)),
      programFill(OCL::createProgramFromFile(device, context, "fill.cl", true)),
      programAdvection(OCL::createProgramFromFile(device, context, "advection.cl", true)),
      programDivergence(OCL::createProgramFromFile(device, context, "pressure/divergence.cl", true)),
      programPressureJacobi(OCL::createProgramFromFile(device, context, "pressure/jacobi.cl", true)),
      programApplyPressure(OCL::createProgramFromFile(device, context, "pressure/apply_pressure.cl", true)),
      programControls(OCL::createProgramFromFile(device, context, "controls.cl", true)),
      kernelFillVelocity(OCL::createKernel(programFill, "fillVelocity")),
      kernelFillColor(OCL::createKernel(programFill, "fillColor")),
      kernelAdvection(OCL::createKernel(programAdvection, "advection3f")),
      kernelDivergence(OCL::createKernel(programDivergence, "calculate_divergence")),
      kernelPressureJacobi(OCL::createKernel(programPressureJacobi, "solve_jacobi_iteration")),
      kernelApplyPressure(OCL::createKernel(programApplyPressure, "apply_pressure")),
      kernelApplyVelocity(OCL::createKernel(programControls, "applyVelocity")),
      kernelStop(OCL::createKernel(programControls, "stop")),
      velocity(context, imageSize, vectorFieldFormat),
      divergence(context, imageSize, scalarFieldFormat),
      pressure(context, imageSize, scalarFieldFormat),
      color(context, imageSize, vectorFieldFormat) {

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
    OCL::setKernelArgMem(kernelApplyVelocity, 0, velocity.getSource());    // inVelocity
    OCL::setKernelArgVec(kernelApplyVelocity, 1, positionX, positionY, 0); // inCenter
    OCL::setKernelArgVec(kernelApplyVelocity, 2, changeX, changeY, 0);     // inVelocityChange
    OCL::setKernelArgFlt(kernelApplyVelocity, 3, radius);                  // inRadius
    OCL::setKernelArgMem(kernelApplyVelocity, 4, velocity.getDestination());
    OCL::enqueueKernel3D(commandQueue, kernelApplyVelocity, imageSize);
    velocity.swap();
}

void Simulation::stop() {
    OCL::setKernelArgMem(kernelStop, 0, velocity.getSource());      // inVelocity
    OCL::setKernelArgMem(kernelStop, 1, velocity.getDestination()); // outVelocity
    OCL::enqueueKernel3D(commandQueue, kernelStop, imageSize);
    velocity.swap();
}
