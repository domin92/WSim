#include "Utils/OpenCL.h"

#include <gtest/gtest.h>

struct KernelTest : ::testing::Test {
    const cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
    const cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};

    void SetUp() {
        platform = OCL::createPlatform();
        device = OCL::createDevice(platform);
        context = OCL::createContext(platform, device);
        queue = OCL::createCommandQueue(context, device);
    }

    cl_kernel createKernel(const std::string &programPath, const std::string &kernelName) {
        program = OCL::createProgram(device, context, programPath, true);
        kernel = OCL::createKernel(program, kernelName.c_str());
        return kernel;
    }

    cl_platform_id platform;
    OCL::Device device;
    OCL::Context context;
    OCL::CommandQueue queue;

    OCL::Program program;
    OCL::Kernel kernel;
};
