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
      programApplyVelocity(OCL::createProgramFromFile(device, context, "apply_velocity.cl", true)),
      kernelFillVelocity(OCL::createKernel(programFill, "fillVelocity")),
      kernelFillColor(OCL::createKernel(programFill, "fillColor")),
      kernelAdvection(OCL::createKernel(programAdvection, "advection3f")),
      kernelDivergence(OCL::createKernel(programDivergence, "calculate_divergence")),
      kernelPressureJacobi(OCL::createKernel(programPressureJacobi, "solve_jacobi_iteration")),
      kernelApplyPressure(OCL::createKernel(programApplyPressure, "apply_pressure")),
      kernelApplyVelocity(OCL::createKernel(programApplyVelocity, "applyVelocity")),
      velocity(context, imageSize, vectorFieldFormat),
      divergence(context, imageSize, scalarFieldFormat),
      pressure(context, imageSize, scalarFieldFormat),
      color(context, imageSize, vectorFieldFormat) {

    // Fill velocity buffer
    OCL::setKernelArgMem(kernelFillVelocity, 0, velocity.getDestination());
    OCL::setKernelArgFlt(kernelFillVelocity, 1, imageSize.x);
    OCL::enqueueKernel3D(commandQueue, kernelFillVelocity, imageSize);
    velocity.swap();

    // Fill color buffer
    OCL::setKernelArgMem(kernelFillColor, 0, color.getDestination());
    OCL::setKernelArgFlt(kernelFillColor, 1, imageSize.x);
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
