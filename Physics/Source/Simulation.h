#pragma once

#include "Utils/ImagePair.h"
#include "Utils/OpenCL.h"

const static cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
const static cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};
class Simulation {
public:
    Simulation(OCL::Vec3 imageSize);
    void stepSimulation(float deltaTime);
    void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) {
        OCL::setKernelArgMem(kernelApplyVelocity, 0, velocity.getSource());    // inVelocity
        OCL::setKernelArgVec(kernelApplyVelocity, 1, positionX, positionY, 0); // inCenter
        OCL::setKernelArgVec(kernelApplyVelocity, 2, changeX, changeY, 0);     // inVelocityChange
        OCL::setKernelArgFlt(kernelApplyVelocity, 3, radius);                  // inRadius
        OCL::setKernelArgMem(kernelApplyVelocity, 4, velocity.getDestination());
        OCL::enqueueKernel3D(commandQueue, kernelApplyVelocity, imageSize);
        velocity.swap();

        auto buff = std::make_unique<float[]>(imageSize.getRequiredBufferSize(16u));
        OCL::enqueueReadImage3D(commandQueue, velocity.getSource(), CL_TRUE, imageSize, 0, 0, buff.get());
    }
    void stop() {
        OCL::setKernelArgMem(kernelStop, 0, velocity.getSource());      // inVelocity
        OCL::setKernelArgMem(kernelStop, 1, velocity.getDestination()); // outVelocity
        OCL::enqueueKernel3D(commandQueue, kernelStop, imageSize);
        velocity.swap();
    }

    auto &getCommandQueue() { return commandQueue; }
    auto &getColor() { return color; }
    auto &getVelocity() { return velocity; }
    auto &getKernelFillVelocity() { return kernelFillVelocity; }
    auto &getKernelFillColor() { return kernelFillColor; }
    auto &getKernelAdvection() { return kernelAdvection; }
    auto &getKernelDivergence() { return kernelDivergence; }
    auto &getKernelPressureJacobi() { return kernelPressureJacobi; }
    auto &getKernelApplyPressure() { return kernelApplyPressure; }

private:
    // General data
    const OCL::Vec3 imageSize;
    cl_platform_id platform;
    OCL::Device device;
    OCL::Context context;
    OCL::CommandQueue commandQueue;

    // Programs
    OCL::Program programFill;
    OCL::Program programAdvection;
    OCL::Program programDivergence;
    OCL::Program programPressureJacobi;
    OCL::Program programApplyPressure;
    OCL::Program programControls;

    // Kernels
    OCL::Kernel kernelFillVelocity;
    OCL::Kernel kernelFillColor;
    OCL::Kernel kernelAdvection;
    OCL::Kernel kernelDivergence;
    OCL::Kernel kernelPressureJacobi;
    OCL::Kernel kernelApplyPressure;
    OCL::Kernel kernelApplyVelocity;
    OCL::Kernel kernelStop;

    // Buffers
    Image3DPair velocity;
    Image3DPair divergence;
    Image3DPair pressure;
    Image3DPair color;
};
