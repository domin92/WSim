#pragma once

#include "Utils/OpenCL.h"

struct Image3DPair {
    explicit Image3DPair(cl_context context, OCL::Vec3 size, const cl_image_format &format) {
        images[0] = OCL::createReadWriteImage3D(context, size, format);
        images[1] = OCL::createReadWriteImage3D(context, size, format);
    }

    OCL::Mem &getSource() { return images[sourceResourceIndex]; }
    OCL::Mem &getDestination() { return images[1 - sourceResourceIndex]; }
    void swap() { sourceResourceIndex = 1 - sourceResourceIndex; }

protected:
    int sourceResourceIndex = 0;
    OCL::Mem images[2] = {};
};

const static cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
const static cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};
class Simulation {
public:
    Simulation(OCL::Vec3 imageSize);
    void stepSimulation(float deltaTime);

    auto &getCommandQueue() { return commandQueue; }
    auto &getColor() { return color; }
    auto &getVelocity() { return velocity; }

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
