#define CL_TARGET_OPENCL_VERSION 120

#include "Tests/MockOpenCL/MockOpenCL.h"

#include <CL/cl.h>
#include <cassert>
#include <type_traits>

template <typename ApiCall, MockOpenCL::MockedCall mockedCall, typename... Args>
auto callMockedBody(Args... args) {
    const auto mockedBody = static_cast<ApiCall>(MockOpenCL::mockedBodies[int(mockedCall)]);
    assert(mockedBody != nullptr);
    return mockedBody(args...);
}

#define CALL_MOCKED_BODY_AND_RETURN(apiCall, ...) \
    return callMockedBody<decltype(&apiCall), MockOpenCL::MockedCall::apiCall>(__VA_ARGS__);

CL_API_ENTRY cl_int CL_API_CALL
clGetPlatformIDs(cl_uint num_entries,
                 cl_platform_id *platforms,
                 cl_uint *num_platforms) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clGetPlatformIDs, num_entries, platforms, num_platforms);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceIDs(cl_platform_id platform,
               cl_device_type device_type,
               cl_uint num_entries,
               cl_device_id *devices,
               cl_uint *num_devices) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clGetDeviceIDs, platform, device_type, num_entries, devices, num_devices);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetPlatformInfo(cl_platform_id platform,
                  cl_platform_info param_name,
                  size_t param_value_size,
                  void *param_value,
                  size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clGetPlatformInfo, platform, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceInfo(cl_device_id device,
                cl_device_info param_name,
                size_t param_value_size,
                void *param_value,
                size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clGetDeviceInfo, device, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_context CL_API_CALL
clCreateContext(const cl_context_properties *properties,
                cl_uint num_devices,
                const cl_device_id *devices,
                void(CL_CALLBACK *pfn_notify)(const char *errinfo,
                                              const void *private_info,
                                              size_t cb,
                                              void *user_data),
                void *user_data,
                cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clCreateContext, properties, num_devices, devices, pfn_notify, user_data, errcode_ret);
}

CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_command_queue CL_API_CALL
clCreateCommandQueue(cl_context context,
                     cl_device_id device,
                     cl_command_queue_properties properties,
                     cl_int *errcode_ret) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED {
    CALL_MOCKED_BODY_AND_RETURN(clCreateCommandQueue, context, device, properties, errcode_ret);
}

CL_API_ENTRY cl_mem CL_API_CALL
clCreateImage(cl_context context,
              cl_mem_flags flags,
              const cl_image_format *image_format,
              const cl_image_desc *image_desc,
              void *host_ptr,
              cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_2 {
    CALL_MOCKED_BODY_AND_RETURN(clCreateImage, context, flags, image_format, image_desc, host_ptr, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clBuildProgram(cl_program program,
               cl_uint num_devices,
               const cl_device_id *device_list,
               const char *options,
               void(CL_CALLBACK *pfn_notify)(cl_program program, void *user_data),
               void *user_data) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clBuildProgram, program, num_devices, device_list, options, pfn_notify, user_data);
}

CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithSource(cl_context context,
                          cl_uint count,
                          const char **strings,
                          const size_t *lengths,
                          cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clCreateProgramWithSource, context, count, strings, lengths, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clGetProgramBuildInfo(cl_program program,
                      cl_device_id device,
                      cl_program_build_info param_name,
                      size_t param_value_size,
                      void *param_value,
                      size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clGetProgramBuildInfo, program, device, param_name, param_value_size, param_value, param_value_size_ret);
}

CL_API_ENTRY cl_kernel CL_API_CALL
clCreateKernel(cl_program program,
               const char *kernel_name,
               cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clCreateKernel, program, kernel_name, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueNDRangeKernel(cl_command_queue command_queue,
                       cl_kernel kernel,
                       cl_uint work_dim,
                       const size_t *global_work_offset,
                       const size_t *global_work_size,
                       const size_t *local_work_size,
                       cl_uint num_events_in_wait_list,
                       const cl_event *event_wait_list,
                       cl_event *event) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clEnqueueNDRangeKernel, command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL
clSetKernelArg(cl_kernel kernel,
               cl_uint arg_index,
               size_t arg_size,
               const void *arg_value) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clSetKernelArg, kernel, arg_index, arg_size, arg_value);
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadImage(cl_command_queue command_queue,
                   cl_mem image,
                   cl_bool blocking_read,
                   const size_t *origin,
                   const size_t *region,
                   size_t row_pitch,
                   size_t slice_pitch,
                   void *ptr,
                   cl_uint num_events_in_wait_list,
                   const cl_event *event_wait_list,
                   cl_event *event) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clEnqueueReadImage, command_queue, image, blocking_read, origin, region, row_pitch, slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteImage(cl_command_queue command_queue,
                    cl_mem image,
                    cl_bool blocking_write,
                    const size_t *origin,
                    const size_t *region,
                    size_t input_row_pitch,
                    size_t input_slice_pitch,
                    const void *ptr,
                    cl_uint num_events_in_wait_list,
                    const cl_event *event_wait_list,
                    cl_event *event) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clEnqueueWriteImage, command_queue, image, blocking_write, origin, region, input_row_pitch, input_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueFillImage(cl_command_queue command_queue,
                   cl_mem image,
                   const void *fill_color,
                   const size_t *origin,
                   const size_t *region,
                   cl_uint num_events_in_wait_list,
                   const cl_event *event_wait_list,
                   cl_event *event) CL_API_SUFFIX__VERSION_1_2 {
    CALL_MOCKED_BODY_AND_RETURN(clEnqueueFillImage, command_queue, image, fill_color, origin, region, num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int CL_API_CALL
clFinish(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clFinish, command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL
clFlush(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clFlush, command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseDevice(cl_device_id device) CL_API_SUFFIX__VERSION_1_2 {
    CALL_MOCKED_BODY_AND_RETURN(clReleaseDevice, device);
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseContext(cl_context context) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clReleaseContext, context);
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseCommandQueue(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clReleaseCommandQueue, command_queue);
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseMemObject(cl_mem memobj) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clReleaseMemObject, memobj);
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseProgram(cl_program program) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clReleaseProgram, program);
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseKernel(cl_kernel kernel) CL_API_SUFFIX__VERSION_1_0 {
    CALL_MOCKED_BODY_AND_RETURN(clReleaseKernel, kernel);
}
