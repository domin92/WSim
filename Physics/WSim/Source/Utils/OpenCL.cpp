#include "OpenCL.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <streambuf>
#include <string>

namespace OCL::detail {
template <typename T>
inline void setKernelArg(cl_kernel kernel, cl_uint argIndex, const T &arg) {
    ASSERT_CL_SUCCESS(clSetKernelArg(kernel, argIndex, sizeof(T), &arg));
}

struct PlatformInfo {
    std::string profile;
    std::string version;
    std::string name;
    std::string vendor;
    std::string extensions;
};

PlatformInfo getPlatformInfo(cl_platform_id platform) {
    char buffer[4096];
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

namespace OCL {
cl_platform_id createPlatform() {
    cl_uint platformsNum{};
    cl_int retVal = clGetPlatformIDs(0, nullptr, &platformsNum);
    ASSERT_CL_SUCCESS(retVal);

    auto platforms = std::make_unique<cl_platform_id[]>(platformsNum);
    retVal = clGetPlatformIDs(platformsNum, platforms.get(), nullptr);
    ASSERT_CL_SUCCESS(retVal);

    const auto platformInfo = detail::getPlatformInfo(platforms[0]);
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
    CommandQueue commandQueue = clCreateCommandQueueWithProperties(context, device, nullptr, &retVal);
    ASSERT_CL_SUCCESS(retVal);
    return commandQueue;
}

Program createProgramFromFile(cl_device_id device, cl_context context, const std::string &sourceFilePath, bool compilationMustSuceed) {
    std::ifstream file(std::string{SHADERS_DIR} + "/" + sourceFilePath);
    if (!file.good()) {
        if (compilationMustSuceed) {
            assert(false);
        }
        return {};
    }

    const std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    return createProgramFromSource(device, context, source, compilationMustSuceed);
}
Program createProgramFromSource(cl_device_id device, cl_context context, const std::string &source, bool compilationMustSuceed) {
    const std::string sourceWithExtensions = "#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable\n" + source;
    const char *sourceString = sourceWithExtensions.c_str();
    const size_t sourceLength = sourceWithExtensions.size();
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

        std::cerr << log.get() << '\n';
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

void setKernelArgMem(cl_kernel kernel, cl_uint argIndex, const Mem &mem) {
    const cl_mem clMem = mem;
    detail::setKernelArg(kernel, argIndex, clMem);
}

void setKernelArgFlt(cl_kernel kernel, cl_uint argIndex, float arg) {
    detail::setKernelArg(kernel, argIndex, arg);
}

void setKernelArgVec(cl_kernel kernel, cl_uint argIndex, float x, float y, float z) {
    float vec4[] = {x, y, z, 0};
    ASSERT_CL_SUCCESS(clSetKernelArg(kernel, argIndex, sizeof(float) * 4, vec4));
}

void setKernelArgInt(cl_kernel kernel, cl_uint argIndex, int arg) {
    detail::setKernelArg(kernel, argIndex, arg);
}

void enqueueReadImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 imageSize, void *outPtr) {
    Vec3 zeros{};
    cl_int retVal = clEnqueueReadImage(commandQueue, image, blocking, zeros.ptr, imageSize.ptr, 0, 0, outPtr, 0, nullptr, nullptr);
    ASSERT_CL_SUCCESS(retVal);
}

void enqueueWriteImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 imageSize, const void *data) {
    Vec3 zeros{};
    cl_int retVal = clEnqueueWriteImage(commandQueue, image, blocking, zeros.ptr, imageSize.ptr, 0, 0, data, 0, nullptr, nullptr);
    ASSERT_CL_SUCCESS(retVal);
}

void enqueueZeroImage3D(cl_command_queue queue, cl_mem image, Vec3 imageSize) {
    Vec3 regionOrigin{};
    float pixel[4] = {0, 0, 0, 0};
    cl_int retVal = clEnqueueFillImage(queue, image, pixel, regionOrigin.ptr, imageSize.ptr, 0, nullptr, nullptr);
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
