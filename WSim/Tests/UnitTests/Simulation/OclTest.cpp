#include "OclTest.h"

const cl_platform_id OclTest::platformId = reinterpret_cast<cl_platform_id>(0x123);
const cl_device_id OclTest::deviceId = reinterpret_cast<cl_device_id>(0x456);
const cl_context OclTest::contextId = reinterpret_cast<cl_context>(0xabc);
const cl_command_queue OclTest::queueId = reinterpret_cast<cl_command_queue>(0xdef);
cl_mem OclTest::memId = reinterpret_cast<cl_mem>(0xaaaa0000ull);
cl_program OclTest::programId = reinterpret_cast<cl_program>(0xbbbb0000ull);
cl_kernel OclTest::kernelId = reinterpret_cast<cl_kernel>(0xcccc0000ull);

template <typename A>
constexpr inline A ptrAdd(A ptr, size_t offset) {
    static_assert(std::is_pointer_v<A>);
    return reinterpret_cast<A>(reinterpret_cast<uintptr_t>(ptr) + static_cast<uintptr_t>(offset));
}

cl_int OclTest::clGetPlatformIDs(cl_uint num_entries,
                                 cl_platform_id *platforms,
                                 cl_uint *num_platforms) {
    EXPECT_EQ((num_entries == 0), (platforms == nullptr));

    // platforms count query, return 1
    if (platforms == nullptr) {
        *num_platforms = 1;
        return CL_SUCCESS;
        return CL_SUCCESS;
    }

    // platform_id query
    EXPECT_EQ(1u, num_entries);
    *platforms = cl_platform_id(0x123);
    return CL_SUCCESS;
}

cl_int OclTest::clGetDeviceIDs(cl_platform_id platform,
                               cl_device_type device_type,
                               cl_uint num_entries,
                               cl_device_id *devices,
                               cl_uint *num_devices) {
    EXPECT_EQ(platformId, platform);
    EXPECT_EQ(CL_DEVICE_TYPE_GPU, device_type);
    EXPECT_EQ((num_entries == 0), (devices == nullptr));

    // devices count query, return 1
    if (devices == nullptr) {
        *num_devices = 1;
        return CL_SUCCESS;
    }

    // device_id query
    EXPECT_EQ(1u, num_entries);
    devices[0] = deviceId;
    return CL_SUCCESS;
}

cl_context OclTest::clCreateContext(const cl_context_properties *properties,
                                    cl_uint num_devices,
                                    const cl_device_id *devices,
                                    void(CL_CALLBACK *pfn_notify)(const char *errinfo,
                                                                  const void *private_info,
                                                                  size_t cb,
                                                                  void *user_data),
                                    void *user_data,
                                    cl_int *errcode_ret) {
    EXPECT_EQ(cl_context_properties(CL_CONTEXT_PLATFORM), properties[0]);
    EXPECT_EQ(cl_context_properties(platformId), properties[1]);
    EXPECT_EQ(cl_context_properties(0u), properties[2]);
    EXPECT_EQ(1u, num_devices);
    EXPECT_EQ(deviceId, devices[0]);

    *errcode_ret = CL_SUCCESS;
    return contextId;
}

cl_command_queue OclTest::clCreateCommandQueue(cl_context context,
                                               cl_device_id device,
                                               cl_command_queue_properties properties,
                                               cl_int *errcode_ret) {
    EXPECT_EQ(contextId, context);
    EXPECT_EQ(deviceId, device);
    EXPECT_EQ(0u, properties);

    *errcode_ret = CL_SUCCESS;
    return queueId;
}

cl_mem OclTest::clCreateImage(cl_context context,
                              cl_mem_flags flags,
                              const cl_image_format *image_format,
                              const cl_image_desc *image_desc,
                              void *host_ptr,
                              cl_int *errcode_ret) {
    EXPECT_EQ(contextId, context);
    EXPECT_EQ(CL_MEM_READ_WRITE, flags);
    EXPECT_EQ(CL_FLOAT, image_format->image_channel_data_type);
    EXPECT_TRUE((CL_RGBA == image_format->image_channel_order) || (CL_R == image_format->image_channel_order));
    EXPECT_EQ(nullptr, host_ptr);

    *errcode_ret = CL_SUCCESS;
    memId = ptrAdd(memId, 1);
    return memId;
}

cl_program OclTest::clCreateProgramWithSource(cl_context context,
                                              cl_uint count,
                                              const char **strings,
                                              const size_t *lengths,
                                              cl_int *errcode_ret) {
    EXPECT_EQ(contextId, context);
    EXPECT_LT(0, count);

    *errcode_ret = CL_SUCCESS;
    programId = ptrAdd(programId, 1);
    return programId;
}

cl_int OclTest::clBuildProgram(cl_program program,
                               cl_uint num_devices,
                               const cl_device_id *device_list,
                               const char *options,
                               void(CL_CALLBACK *pfn_notify)(cl_program program, void *user_data),
                               void *user_data) {
    // TODO check program
    EXPECT_EQ(1u, num_devices);
    EXPECT_EQ(deviceId, device_list[0]);
    return CL_SUCCESS;
}

cl_kernel OclTest::clCreateKernel(cl_program program,
                                  const char *kernel_name,
                                  cl_int *errcode_ret) {
    // TODO check program

    *errcode_ret = CL_SUCCESS;
    kernelId = ptrAdd(kernelId, 1);
    return kernelId;
}

cl_int OclTest::clEnqueueFillImage(cl_command_queue command_queue,
                                   cl_mem image,
                                   const void *fill_color,
                                   const size_t *origin,
                                   const size_t *region,
                                   cl_uint num_events_in_wait_list,
                                   const cl_event *event_wait_list,
                                   cl_event *event) {
    EXPECT_EQ(queueId, command_queue);
    // TODO check image
    EXPECT_NE(0, region[0]); // All fills should be 3D
    EXPECT_NE(0, region[1]); // All fills should be 3D
    EXPECT_NE(0, region[2]); // All fills should be 3D
    return CL_SUCCESS;
}

cl_int OclTest::clSetKernelArg(cl_kernel kernel,
                               cl_uint arg_index,
                               size_t arg_size,
                               const void *arg_value) {
    // TODO check kernel
    EXPECT_LT(0u, arg_size);
    EXPECT_NE(nullptr, arg_value);
    return CL_SUCCESS;
}

cl_int OclTest::clEnqueueNDRangeKernel(cl_command_queue command_queue,
                                       cl_kernel kernel,
                                       cl_uint work_dim,
                                       const size_t *global_work_offset,
                                       const size_t *global_work_size,
                                       const size_t *local_work_size,
                                       cl_uint num_events_in_wait_list,
                                       const cl_event *event_wait_list,
                                       cl_event *event) {
    EXPECT_EQ(queueId, command_queue);
    // TODO check kernel
    EXPECT_EQ(3u, work_dim);
    EXPECT_NE(0u, global_work_size[0]); // All kernels should be 3D
    EXPECT_NE(0u, global_work_size[1]); // All kernels should be 3D
    EXPECT_NE(0u, global_work_size[2]); // All kernels should be 3D
    return CL_SUCCESS;
}

cl_int OclTest::clReleaseDevice(cl_device_id device) {
    EXPECT_EQ(deviceId, device);
    return CL_SUCCESS;
}

cl_int OclTest::clReleaseContext(cl_context context) {
    EXPECT_EQ(contextId, context);
    return CL_SUCCESS;
}

cl_int OclTest::clReleaseCommandQueue(cl_command_queue command_queue) {
    EXPECT_EQ(queueId, command_queue);
    return CL_SUCCESS;
}

cl_int OclTest::clReleaseMemObject(cl_mem memobj) {
    // TODO check memobj
    return CL_SUCCESS;
}

cl_int OclTest::clReleaseProgram(cl_program program) {
    // TODO check program
    return CL_SUCCESS;
}

cl_int OclTest::clReleaseKernel(cl_kernel kernel) {
    // TOOD check kernel
    return CL_SUCCESS;
}
