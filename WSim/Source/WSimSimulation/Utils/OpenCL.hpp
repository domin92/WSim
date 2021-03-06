#pragma once

#define CL_TARGET_OPENCL_VERSION 120

#include "Source/WSimCommon/Error.hpp"
#include "Source/WSimCommon/Logger.hpp"
#include "Source/WSimCommon/Vec3.hpp"

#include <CL/cl.h>
#include <iostream>
#include <string>

#define ASSERT_CL_SUCCESS(retVal)                                                                                 \
    if (retVal != CL_SUCCESS) {                                                                                   \
        Logger::get() << __FILE__ << ":" << __LINE__ << " (" << __FUNCTION__ << "): retVal = " << retVal << '\n'; \
        wsimError();                                                                                              \
    }

#define DEFINE_RAII_WRAPPER(name, clType, releaseMethod)          \
    class name {                                                  \
    protected:                                                    \
        clType value;                                             \
                                                                  \
    public:                                                       \
        name() = default;                                         \
        name(clType value) : value(value) {}                      \
        name(const name &) = delete;                              \
        name &operator=(const name &) = delete;                   \
        name(name &&other) noexcept { *this = std::move(other); } \
        name &operator=(name &&other) noexcept {                  \
            this->value = other.value;                            \
            other.value = 0;                                      \
            return *this;                                         \
        }                                                         \
        ~name() {                                                 \
            if (value != 0) {                                     \
                ASSERT_CL_SUCCESS(releaseMethod(value));          \
            }                                                     \
        }                                                         \
        operator clType() const { return value; }                 \
    };

namespace OCL {
// RAII Wrappers for cl objects
DEFINE_RAII_WRAPPER(Device, cl_device_id, clReleaseDevice);
DEFINE_RAII_WRAPPER(Context, cl_context, clReleaseContext);
DEFINE_RAII_WRAPPER(CommandQueue, cl_command_queue, clReleaseCommandQueue);
DEFINE_RAII_WRAPPER(Mem, cl_mem, clReleaseMemObject);
DEFINE_RAII_WRAPPER(Program, cl_program, clReleaseProgram);
DEFINE_RAII_WRAPPER(Kernel, cl_kernel, clReleaseKernel);

// Querying info
struct PlatformInfo {
    std::string profile;
    std::string version;
    std::string name;
    std::string vendor;
    std::string extensions;
};
struct DeviceInfo {
    std::string name;
    std::string profile;
    std::string extensions;
    cl_device_type deviceType;
    Vec3 image3DMaxSize;
    size_t maxComputeUnits;
};
PlatformInfo getPlatformInfo(cl_platform_id platform);
DeviceInfo getDeviceInfo(cl_device_id device);

// Init
cl_platform_id createPlatform(size_t index);
Device createDevice(cl_platform_id platform, cl_device_type deviceType, size_t index);
Context createContext(cl_platform_id platform, cl_device_id device);
CommandQueue createCommandQueue(cl_context context, cl_device_id device);

// Compile kernels
Program createProgramFromFile(cl_device_id device, cl_context context, const std::string &sourceFilePath, bool compilationMustSuceed, const std::string &sourcePrefix);
Program createProgramFromSource(cl_device_id device, cl_context context, const std::string &source, bool compilationMustSuceed, const std::string& sourcePrefix);
Kernel createKernel(cl_program program, const char *kernelName);

// Enqueue kernels
void enqueueKernel3D(cl_command_queue commandQueue, cl_kernel kernel, Vec3 globalWorkOffset, Vec3 globalWorkSize);
void enqueueKernel3D(cl_command_queue commandQueue, cl_kernel kernel, Vec3 globalWorkSize);
void setKernelArgMem(cl_kernel kernel, cl_uint argIndex, const Mem &mem);
void setKernelArgFlt(cl_kernel kernel, cl_uint argIndex, float arg);
void setKernelArgVec(cl_kernel kernel, cl_uint argIndex, float x, float y, float z);
void setKernelArgVec(cl_kernel kernel, cl_uint argIndex, size_t x, size_t y, size_t z);
void setKernelArgInt(cl_kernel kernel, cl_uint argIndex, int arg);

// Enqueue builtins
void enqueueReadImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 imageSize, void *outPtr);
void enqueueReadImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 origin, Vec3 size, void *outPtr);
void enqueueWriteImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 imageSize, const void *data);
void enqueueWriteImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 origin, Vec3 imageSize, const void *data);
void enqueueFillImage3D(cl_command_queue queue, cl_mem image, const void *pattern, Vec3 offset, Vec3 region);
void enqueueZeroImage3D(cl_command_queue queue, cl_mem image, Vec3 imageSize);

// Task submission
void finish(cl_command_queue commandQueue);
void flush(cl_command_queue commandQueue);

// Create allocations
Mem createReadWriteImage3D(cl_context context, Vec3 size, const cl_image_format &format);

// Utility helpers
constexpr inline size_t getChannelCount(cl_channel_order channelOrder) {
    switch (channelOrder) {
    case CL_R:
        return 1;
    case CL_RGBA:
        return 4;
    default:
        wsimError();
    }
}
constexpr inline size_t getSizeOfChannel(cl_channel_type dataType) {
    switch (dataType) {
    case CL_FLOAT:
        return sizeof(cl_float);
    default:
        wsimError();
    }
}
constexpr inline size_t getSizeOfTexel(cl_image_format format) {
    return getChannelCount(format.image_channel_order) * getSizeOfChannel(format.image_channel_data_type);
}

} // namespace OCL

#undef DEFINE_RAII_WRAPPER
