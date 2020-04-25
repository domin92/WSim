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

} // namespace OCL::detail

namespace OCL {

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

DeviceInfo getDeviceInfo(cl_device_id device) {
    char buffer[4096];
    size_t actualSize{};
    DeviceInfo info{};
    cl_int retVal{};

    retVal = clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buffer), buffer, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    info.name = std::string{buffer, actualSize};

    retVal = clGetDeviceInfo(device, CL_DEVICE_PROFILE, sizeof(buffer), buffer, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    info.profile = std::string{buffer, actualSize};

    retVal = clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, sizeof(buffer), buffer, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    info.extensions = std::string{buffer, actualSize};

    retVal = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &info.deviceType, &actualSize);
    ASSERT_CL_SUCCESS(retVal);

    retVal = clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &info.image3DMaxSize.x, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    retVal = clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &info.image3DMaxSize.y, &actualSize);
    ASSERT_CL_SUCCESS(retVal);
    retVal = clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &info.image3DMaxSize.z, &actualSize);
    ASSERT_CL_SUCCESS(retVal);

    retVal = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(size_t), &info.maxComputeUnits, &actualSize);
    ASSERT_CL_SUCCESS(retVal);

    return info;
}

cl_platform_id createPlatform(size_t index) {
    cl_uint platformsNum{};
    cl_int retVal = clGetPlatformIDs(0, nullptr, &platformsNum);
    ASSERT_CL_SUCCESS(retVal);

    if (index >= platformsNum) {
        return nullptr;
    }

    auto platforms = std::make_unique<cl_platform_id[]>(platformsNum);
    retVal = clGetPlatformIDs(platformsNum, platforms.get(), nullptr);
    ASSERT_CL_SUCCESS(retVal);

    return platforms[index];
}

Device createDevice(cl_platform_id platform, cl_device_type deviceType, size_t index) {
    cl_uint devicesNum{};
    cl_int retVal = clGetDeviceIDs(platform, deviceType, 0, nullptr, &devicesNum);
    ASSERT_CL_SUCCESS(retVal);

    if (index > devicesNum) {
        return nullptr;
    }

    auto devices = std::make_unique<cl_device_id[]>(devicesNum);
    retVal = clGetDeviceIDs(platform, deviceType, devicesNum, devices.get(), nullptr);
    ASSERT_CL_SUCCESS(retVal);

    return devices[index];
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

Program createProgramFromFile(cl_device_id device, cl_context context, const std::string &sourceFilePath, bool compilationMustSuceed) {
    std::ifstream file(std::string{SHADERS_DIR} + "/" + sourceFilePath);
    if (!file.good()) {
        wsimErrorIf(compilationMustSuceed);
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
        wsimErrorIf(compilationMustSuceed);
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

void enqueueKernel3D(cl_command_queue commandQueue, cl_kernel kernel, Vec3 globalWorkOffset, Vec3 globalWorkSize) {
    cl_int retVal = clEnqueueNDRangeKernel(commandQueue, kernel, 3,
                                           globalWorkOffset.ptr, globalWorkSize.ptr, nullptr,
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

void setKernelArgVec(cl_kernel kernel, cl_uint argIndex, size_t x, size_t y, size_t z) {
    size_t vec4[] = {x, y, z, 0u};
    ASSERT_CL_SUCCESS(clSetKernelArg(kernel, argIndex, sizeof(int) * 4, vec4));
}

void setKernelArgInt(cl_kernel kernel, cl_uint argIndex, int arg) {
    detail::setKernelArg(kernel, argIndex, arg);
}

void enqueueReadImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 imageSize, void *outPtr) {
    Vec3 zeros{};
    cl_int retVal = clEnqueueReadImage(commandQueue, image, blocking, zeros.ptr, imageSize.ptr, 0, 0, outPtr, 0, nullptr, nullptr);
    ASSERT_CL_SUCCESS(retVal);
}

void enqueueReadImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 origin, Vec3 size, void *outPtr) {
    cl_int retVal = clEnqueueReadImage(commandQueue, image, blocking, origin.ptr, size.ptr, 0, 0, outPtr, 0, nullptr, nullptr);
    ASSERT_CL_SUCCESS(retVal);
}

void enqueueWriteImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 origin, Vec3 imageSize, const void *data) {
    cl_int retVal = clEnqueueWriteImage(commandQueue, image, blocking, origin.ptr, imageSize.ptr, 0, 0, data, 0, nullptr, nullptr);
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
    desc.buffer = nullptr;

    cl_int retVal{};
    Mem image = clCreateImage(context, CL_MEM_READ_WRITE, &format, &desc, nullptr, &retVal);
    ASSERT_CL_SUCCESS(retVal);
    return image;
}
} // namespace OCL
