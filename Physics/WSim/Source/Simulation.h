#pragma once

#include "Utils/ImagePair.h"
#include "Utils/OpenCL.h"

#include <vector>

const static cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
const static cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};
class Simulation {
public:
    Simulation(OCL::Vec3 imageSize);
    void stepSimulation(float deltaTime);
    void applyForce(float positionX, float positionY, float changeX, float changeY, float radius);
    void stop();

    auto getSimulationSize() const { return imageSize; }
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

    // Buffers
    Image3DPair velocity;
    Image3DPair divergence;
    Image3DPair pressure;
    Image3DPair color;

    // Kernels
    std::vector<OCL::Program> programs{}; // held so all objects are properly free
    OCL::Kernel kernelFillVelocity;
    OCL::Kernel kernelFillColor;
    OCL::Kernel kernelAdvection;
    OCL::Kernel kernelDivergence;
    OCL::Kernel kernelPressureJacobi;
    OCL::Kernel kernelApplyPressure;
    OCL::Kernel kernelAddVelocity;
};
