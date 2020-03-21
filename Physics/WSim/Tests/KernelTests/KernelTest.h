#include "Utils/OpenCL.h"

#include <gtest/gtest.h>
#include <vector>

struct KernelTest : ::testing::Test {
    const cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
    const cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};

    void SetUp() {
        platform = OCL::createPlatform();
        device = OCL::createDevice(platform);
        context = OCL::createContext(platform, device);
        queue = OCL::createCommandQueue(context, device);
    }

    void TearDown() {
        programs.clear();
        kernels.clear();
    }

    cl_kernel createKernelFromFile(const std::string &programPath, const std::string &kernelName) {
        return createKernelImpl(OCL::createProgramFromFile(device, context, programPath, true), kernelName);
    }

    cl_kernel createKernelFromSource(const std::string &programSource, const std::string &kernelName) {
        return createKernelImpl(OCL::createProgramFromSource(device, context, programSource, true), kernelName);
    }

    cl_platform_id platform;
    OCL::Device device;
    OCL::Context context;
    OCL::CommandQueue queue;

private:
    cl_kernel createKernelImpl(OCL::Program &&program, const std::string &kernelName) {
        auto kernel = OCL::createKernel(program, kernelName.c_str());

        // Cache the objects, so they are released by TearDown
        programs.push_back(std::move(program));
        kernels.push_back(std::move(kernel));

        // Local variable is invalid after move so take it from the cache
        return static_cast<cl_kernel>(kernels.back());
    }

    std::vector<OCL::Program> programs{};
    std::vector<OCL::Kernel> kernels{};
};
