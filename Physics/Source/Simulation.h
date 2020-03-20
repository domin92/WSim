#pragma once

#include "Utils/ImagePair.h"
#include "Utils/OpenCL.h"

const static cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
const static cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};
class Simulation {
public:
    Simulation(OCL::Vec3 imageSize);
    void stepSimulation(float deltaTime);

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

    // Kernels
    OCL::Kernel kernelFillVelocity;
    OCL::Kernel kernelFillColor;
    OCL::Kernel kernelAdvection;
    OCL::Kernel kernelDivergence;
    OCL::Kernel kernelPressureJacobi;
    OCL::Kernel kernelApplyPressure;

    // Buffers
    Image3DPair velocity;
    Image3DPair divergence;
    Image3DPair pressure;
    Image3DPair color;
};
