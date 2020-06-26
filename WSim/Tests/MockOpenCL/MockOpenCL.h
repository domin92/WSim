#pragma once

namespace MockOpenCL {

enum class MockedCall {
    clGetPlatformIDs = 0,
    clGetPlatformInfo,
    clGetDeviceIDs,
    clGetDeviceInfo,
    clCreateContext,
    clCreateCommandQueue,
    clCreateImage,
    clCreateProgramWithSource,
    clBuildProgram,
    clGetProgramBuildInfo,
    clCreateKernel,
    clEnqueueNDRangeKernel,
    clSetKernelArg,
    clEnqueueReadImage,
    clEnqueueWriteImage,
    clEnqueueFillImage,
    clFinish,
    clFlush,
    clReleaseDevice,
    clReleaseContext,
    clReleaseCommandQueue,
    clReleaseKernel,
    clReleaseProgram,
    clReleaseMemObject,
    COUNT,
};

extern void *mockedBodies[int(MockedCall::COUNT)];

void initialize();

template <MockedCall mockedCall, typename MockedBody>
void mock(MockedBody mockedBody) {
    mockedBodies[int(mockedCall)] = reinterpret_cast<void*>(mockedBody);
}

}; // namespace MockOpenCL
