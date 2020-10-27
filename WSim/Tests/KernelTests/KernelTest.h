#include "Source/WSimSimulation/Utils/OpenCL.hpp"

#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

#define EXPECT_MEM_EQ(expectedMem, actualMem, size) EXPECT_EQ(0, memcmp((expectedMem), (actualMem), size))
#define ASSERT_MEM_EQ(expectedMem, actualMem, size) ASSERT_EQ(0, memcmp((expectedMem), (actualMem), size))

#define EXPECT_FLOATS_EQ(expectedFloats, actualFloats, count, epsilon)        \
    {                                                                         \
        for (auto i = 0u; i < count; i++) {                                   \
            const auto diff = std::fabs(expectedFloats[i] - actualFloats[i]); \
            if (diff > epsilon) {                                             \
                std::cerr << "Float index = " << i << "\n";                   \
                EXPECT_LE(diff, epsilon);                                     \
                break;                                                        \
            }                                                                 \
        }                                                                     \
    }

struct KernelTest : ::testing::Test {
    const cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
    const cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};

    void SetUp() {
        platform = OCL::createPlatform(0u);
        device = OCL::createDevice(platform, CL_DEVICE_TYPE_GPU, 0u);
        context = OCL::createContext(platform, device);
        queue = OCL::createCommandQueue(context, device);
    }

    void TearDown() {
        programs.clear();
        kernels.clear();
    }

    cl_kernel createKernelFromFile(const std::string &programPath, const std::string &kernelName) {
        return createKernelImpl(OCL::createProgramFromFile(device, context, programPath, true, ""), kernelName);
    }

    cl_kernel createKernelFromSource(const std::string &programSource, const std::string &kernelName) {
        return createKernelImpl(OCL::createProgramFromSource(device, context, programSource, true, ""), kernelName);
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
