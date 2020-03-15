#pragma once

#define CL_TARGET_OPENCL_VERSION 200
#include <CL/cl.h>
#include <string>
#include <assert.h>

#define ASSERT_CL_SUCCESS(retVal) assert(retVal == CL_SUCCESS);

namespace OCL {
// RAII Wrappers
template <typename ClType>
class RaiiWrapper {
protected:
    ClType value;

public:
    RaiiWrapper() = default;
    RaiiWrapper(ClType value) : value(value) {}
    RaiiWrapper(const RaiiWrapper &) = delete;
    RaiiWrapper &operator=(const RaiiWrapper &) = delete;
    RaiiWrapper(RaiiWrapper &&other) : value(other.value) { other.value = 0; }
    RaiiWrapper &operator=(RaiiWrapper &&other) {
        value = other.value;
        other.value = 0;
        return *this;
    }
    operator ClType() const { return value; }
    virtual ~RaiiWrapper() {}
};

#define DEFINE_RAII_WRAPPER(name, clType, releaseMethod) \
    struct name : RaiiWrapper<clType> {                  \
        name() = default;                                \
        name(name &&) = default;                         \
        name &operator=(name &&) = default;              \
        name(clType value) : RaiiWrapper(value) {}       \
        ~name() override {                               \
            if (value != 0) {                            \
                ASSERT_CL_SUCCESS(releaseMethod(value)); \
            }                                            \
        }                                                \
    };

DEFINE_RAII_WRAPPER(Device, cl_device_id, clReleaseDevice);
DEFINE_RAII_WRAPPER(Context, cl_context, clReleaseContext);
DEFINE_RAII_WRAPPER(CommandQueue, cl_command_queue, clReleaseCommandQueue);
DEFINE_RAII_WRAPPER(Mem, cl_mem, clReleaseMemObject);
DEFINE_RAII_WRAPPER(Program, cl_program, clReleaseProgram);
DEFINE_RAII_WRAPPER(Kernel, cl_kernel, clReleaseKernel);

// Init
cl_platform_id createPlatform();
Device createDevice(cl_platform_id platform);
Context createContext(cl_platform_id platform, cl_device_id device);
CommandQueue createCommandQueue(cl_context context, cl_device_id device);

// Compile kernels
Program createProgram(cl_device_id device, cl_context context, const std::string &sourceFilePath, bool compilationMustSuceed);
Kernel createKernel(cl_program program, const char *kernelName);

// Enqueue kernels
struct Vec3 {
    Vec3() = default;
    Vec3(size_t x, size_t y, size_t z) : x(x), y(y), z(z) {}
    union {
        struct {
            size_t x, y, z;
        };
        size_t ptr[3];
    };
    size_t getRequiredBufferSize(size_t pixelSize) const {
        return x * y * z * pixelSize;
    }
};
void enqueueKernel3D(cl_command_queue commandQueue, cl_kernel kernel, Vec3 globalWorkOffset, Vec3 globalWorkSize, Vec3 localWorkSize);
void enqueueKernel3D(cl_command_queue commandQueue, cl_kernel kernel, Vec3 globalWorkSize);
void setKernelArgMem(cl_kernel kernel, cl_uint argIndex, const Mem &mem);
void setKernelArgFlt(cl_kernel kernel, cl_uint argIndex, float arg);
void setKernelArgInt(cl_kernel kernel, cl_uint argIndex, int arg);

// Enqueue builtins
void enqueueReadImage3D(cl_command_queue commandQueue, cl_mem image, cl_bool blocking, Vec3 imageSize, size_t outRowPitch, size_t outSlicePitch, void *outPtr);

// Misc
size_t calculateSizeOfImage3D(Vec3 imageSize, size_t rowPitch, size_t slicePitch);
void finish(cl_command_queue commandQueue);
void flush(cl_command_queue commandQueue);

// Create allocations
Mem createReadWriteImage3D(cl_context context, Vec3 size, const cl_image_format &format);

} // namespace OCL

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
PlatformInfo getPlatformInfo(cl_platform_id platform);
} // namespace OCL::detail
