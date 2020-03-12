#include "OpenCL.h"

#include <memory>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

namespace OCL {
cl_platform_id createPlatform() {
    cl_uint platformsNum{};
    cl_int retVal = clGetPlatformIDs(0, nullptr, &platformsNum);
    ASSERT_CL_SUCCESS(retVal);

    auto platforms = std::make_unique<cl_platform_id[]>(platformsNum);
    retVal = clGetPlatformIDs(platformsNum, platforms.get(), nullptr);
    ASSERT_CL_SUCCESS(retVal);

    auto a = detail::getPlatformInfo(platforms[0]);
    auto b = detail::getPlatformInfo(platforms[1]);
    auto c = detail::getPlatformInfo(platforms[2]);

    return platforms[0];
}

Device createDevice(cl_platform_id platform) {
    cl_uint devicesNum{};
    cl_int retVal = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &devicesNum);
    ASSERT_CL_SUCCESS(retVal);

    auto devices = std::make_unique<cl_device_id[]>(devicesNum);
    retVal = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, devicesNum, devices.get(), nullptr);
    ASSERT_CL_SUCCESS(retVal);

    return devices[0];
}

Context createContext(cl_platform_id platform, cl_device_id device) {
    cl_context_properties properties[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
        0};

    cl_int retVal{};
    Context context = clCreateContext(properties, 1, &device, nullptr, nullptr, &retVal);
    ASSERT_CL_SUCCESS(retVal);

    return context;
}

CommandQueue createCommandQueue(cl_context context, cl_device_id device) {
    cl_int retVal{};
    CommandQueue commandQueue = clCreateCommandQueue(context, device, 0, &retVal);
    ASSERT_CL_SUCCESS(retVal);
    return commandQueue;
}

// Compile kernels
Program createProgram(cl_device_id device, cl_context context, const std::string &sourceFilePath, bool compilationMustSuceed) {
    std::ifstream file(std::string{SHADERS_DIR} + "/" + sourceFilePath);
    if (!file.good()) {
        if (compilationMustSuceed) {
            assert(false);
        }
        return {};
    }

    const std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    const char *sourceString = source.c_str();
    const size_t sourceLength = source.size();
    cl_int retVal{};
    Program program = clCreateProgramWithSource(context, 1, &sourceString, &sourceLength, &retVal);
    ASSERT_CL_SUCCESS(retVal);

    retVal = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (retVal != CL_SUCCESS) {
        size_t length{};
        retVal = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &length);
        ASSERT_CL_SUCCESS(retVal);

        auto log = std::make_unique<char[]>(length);
        retVal = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, length, log.get(), nullptr);
        ASSERT_CL_SUCCESS(retVal);

        std::cerr << log << '\n';
        if (compilationMustSuceed) {
            assert(false);
        }
        return {};
    }

    return program;
}
Kernel createKernel(cl_program program, const char *kernelName) {
    cl_int retVal{};
    Kernel kernel = clCreateKernel(program, kernelName, &retVal);
    ASSERT_CL_SUCCESS(retVal);
    return kernel;
}

void enqueueKernel3D(cl_command_queue commandQueue, cl_kernel kernel, Vec3 globalWorkOffset, Vec3 globalWorkSize, Vec3 localWorkSize) {
    cl_int retVal = clEnqueueNDRangeKernel(commandQueue, kernel, 3,
                                           globalWorkOffset.ptr, globalWorkSize.ptr, localWorkSize.ptr,
                                           0, nullptr, nullptr);
    ASSERT_CL_SUCCESS(retVal);
}

void enqueueKernel3D(cl_command_queue commandQueue, cl_kernel kernel, Vec3 globalWorkSize) {
    cl_int retVal = clEnqueueNDRangeKernel(commandQueue, kernel, 3,
                                           nullptr, globalWorkSize.ptr, nullptr,
                                           0, nullptr, nullptr);
    ASSERT_CL_SUCCESS(retVal);
}

void enqueueReadImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 imageSize, size_t outRowPitch, size_t outSlicePitch, void *outPtr) {
    Vec3 zeros{};
    cl_int retVal = clEnqueueReadImage(commandQueue, image, blocking, zeros.ptr, imageSize.ptr, outRowPitch, outSlicePitch, outPtr, 0, nullptr, nullptr);
    ASSERT_CL_SUCCESS(retVal);
}

size_t calculateSizeOfImage3D(Vec3 imageSize, size_t rowPitch, size_t slicePitch)
{
    assert(rowPitch >= imageSize.x);
    assert(slicePitch >= rowPitch * imageSize.y);
    return slicePitch * imageSize.z;
}

void finish(cl_command_queue commandQueue) {
    ASSERT_CL_SUCCESS(clFinish(commandQueue));
}

void flush(cl_command_queue commandQueue) {
    ASSERT_CL_SUCCESS(clFlush(commandQueue));
}

Mem createReadWriteImage3D(cl_context context, Vec3 size, const cl_image_format &format) {
    cl_image_desc desc{};
    desc.image_type = CL_MEM_OBJECT_IMAGE3D;
    desc.image_width = size.x;
    desc.image_height = size.y;
    desc.image_depth = size.z;
    desc.image_array_size = 0;
    desc.image_row_pitch = 0;
    desc.image_slice_pitch = 0;
    desc.num_mip_levels = 0;
    desc.num_samples = 0;
    desc.mem_object = nullptr;

    cl_int retVal{};
    Mem image = clCreateImage(context, CL_MEM_READ_WRITE, &format, &desc, nullptr, &retVal);
    ASSERT_CL_SUCCESS(retVal);
    return image;
}
} // namespace OCL

namespace OCL::detail {
PlatformInfo getPlatformInfo(cl_platform_id platform) {
    char buffer[1024];
    size_t actualSize{};
    PlatformInfo info{};
    cl_int retVal{};

    retVal = clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, sizeof(buffer), buffer, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    info.profile = std::string{buffer, actualSize};

    retVal = clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(buffer), buffer, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    info.version = std::string{buffer, actualSize};

    retVal = clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(buffer), buffer, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    info.name = std::string{buffer, actualSize};

    retVal = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(buffer), buffer, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    info.vendor = std::string{buffer, actualSize};

    retVal = clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, sizeof(buffer), buffer, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    info.extensions = std::string{buffer, actualSize};

    return info;
}
} // namespace OCL::detail
