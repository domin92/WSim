#pragma once

#include "Utils/ImagePair.h"
#include "Utils/OpenCL.h"

#include <vector>

const static cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
const static cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};
class Simulation {
public:
    Simulation(OCL::Vec3 simulationSize, size_t borderWidth);
    void stepSimulation(float deltaTime);
    void applyForce(float positionX, float positionY, float changeX, float changeY, float radius);
    void stop();

    auto getBorderOffset() const { return borderOffset; }
    auto getSimulationSize() const { return simulationSize; }
    auto getSimulationSizeWithBorder() const { return simulationSizeWithBorder; }
    auto &getCommandQueue() { return commandQueue; }
    auto &getColor() { return color; }
    auto &getVelocity() { return velocity; }
    auto &getKernelFillVelocity() { return kernelFillVelocity; }
    auto &getKernelFillColor() { return kernelFillColor; }
    auto &getKernelAdvection() { return kernelAdvection; }
    auto &getKernelDivergence() { return kernelDivergence; }
    auto &getKernelPressureJacobi() { return kernelPressureJacobi; }
    auto &getKernelProjectVelocityToDivergenceFree() { return kernelProjectVelocityToDivergenceFree; }

private:
    // Sizes
    const OCL::Vec3 borderOffset;             // offset to be applied to extended images
    const OCL::Vec3 simulationSize;           // size for which simulation kernels are launched
    const OCL::Vec3 simulationSizeWithBorder; // simulationSize increased by additional border space

    // Basic OCL objects
    cl_platform_id platform;
    OCL::Device device;
    OCL::Context context;
    OCL::CommandQueue commandQueue;

    // Images. Images can be private to the node (size=simulationSize) or extended (size=simulationSizeWithBorder),
    // meaning they contain border pixels passed from neighbouring nodes.
    Image3DPair velocity; // extended
    Image3DPair color;    // extended
    Image3DPair divergence;
    Image3DPair pressure;

    // Kernels
    std::vector<OCL::Program> programs{}; // held so all objects are properly free
    OCL::Kernel kernelFillVelocity;
    OCL::Kernel kernelFillColor;
    OCL::Kernel kernelAdvection;
    OCL::Kernel kernelDivergence;
    OCL::Kernel kernelPressureJacobi;
    OCL::Kernel kernelProjectVelocityToDivergenceFree;
    OCL::Kernel kernelAddVelocity;
};
