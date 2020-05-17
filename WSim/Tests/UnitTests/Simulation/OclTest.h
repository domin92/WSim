#pragma once

#define CL_TARGET_OPENCL_VERSION 120

#include "Tests/MockOpenCL/MockOpenCL.h"

#include <CL/cl.h>
#include <gtest/gtest.h>

// This class mocks OpenCL calls for common, positive scenarios. It can be used as a base
// before overriding various calls to get specific effects.
struct OclTest : ::testing::Test {
    static const cl_platform_id platformId;
    static const cl_device_id deviceId;
    static const cl_context contextId;
    static const cl_command_queue queueId;
    static cl_mem memId;
    static cl_program programId;
    static cl_kernel kernelId;

    void SetUp() override {
        MockOpenCL::initialize();
        MockOpenCL::mock<MockOpenCL::MockedCall::clGetPlatformIDs>(clGetPlatformIDs);
        MockOpenCL::mock<MockOpenCL::MockedCall::clGetDeviceIDs>(clGetDeviceIDs);
        MockOpenCL::mock<MockOpenCL::MockedCall::clCreateContext>(clCreateContext);
        MockOpenCL::mock<MockOpenCL::MockedCall::clCreateCommandQueue>(clCreateCommandQueue);
        MockOpenCL::mock<MockOpenCL::MockedCall::clCreateImage>(clCreateImage);
        MockOpenCL::mock<MockOpenCL::MockedCall::clCreateProgramWithSource>(clCreateProgramWithSource);
        MockOpenCL::mock<MockOpenCL::MockedCall::clBuildProgram>(clBuildProgram);
        MockOpenCL::mock<MockOpenCL::MockedCall::clCreateKernel>(clCreateKernel);
        MockOpenCL::mock<MockOpenCL::MockedCall::clEnqueueFillImage>(clEnqueueFillImage);
        MockOpenCL::mock<MockOpenCL::MockedCall::clSetKernelArg>(clSetKernelArg);
        MockOpenCL::mock<MockOpenCL::MockedCall::clEnqueueNDRangeKernel>(clEnqueueNDRangeKernel);

        MockOpenCL::mock<MockOpenCL::MockedCall::clReleaseDevice>(clReleaseDevice);
        MockOpenCL::mock<MockOpenCL::MockedCall::clReleaseContext>(clReleaseContext);
        MockOpenCL::mock<MockOpenCL::MockedCall::clReleaseCommandQueue>(clReleaseCommandQueue);
        MockOpenCL::mock<MockOpenCL::MockedCall::clReleaseKernel>(clReleaseKernel);
        MockOpenCL::mock<MockOpenCL::MockedCall::clReleaseProgram>(clReleaseProgram);
        MockOpenCL::mock<MockOpenCL::MockedCall::clReleaseMemObject>(clReleaseMemObject);
    }

    static cl_int clGetPlatformIDs(cl_uint num_entries,
                                   cl_platform_id *platforms,
                                   cl_uint *num_platforms);
    static cl_int clGetDeviceIDs(cl_platform_id platform,
                                 cl_device_type device_type,
                                 cl_uint num_entries,
                                 cl_device_id *devices,
                                 cl_uint *num_devices);
    static cl_context clCreateContext(const cl_context_properties *properties,
                                      cl_uint num_devices,
                                      const cl_device_id *devices,
                                      void(CL_CALLBACK *pfn_notify)(const char *errinfo,
                                                                    const void *private_info,
                                                                    size_t cb,
                                                                    void *user_data),
                                      void *user_data,
                                      cl_int *errcode_ret);
    static cl_command_queue clCreateCommandQueue(cl_context context,
                                                 cl_device_id device,
                                                 cl_command_queue_properties properties,
                                                 cl_int *errcode_ret);
    static cl_mem clCreateImage(cl_context context,
                                cl_mem_flags flags,
                                const cl_image_format *image_format,
                                const cl_image_desc *image_desc,
                                void *host_ptr,
                                cl_int *errcode_ret);
    static cl_program clCreateProgramWithSource(cl_context context,
                                                cl_uint count,
                                                const char **strings,
                                                const size_t *lengths,
                                                cl_int *errcode_ret);

    static cl_int clBuildProgram(cl_program program,
                                 cl_uint num_devices,
                                 const cl_device_id *device_list,
                                 const char *options,
                                 void(CL_CALLBACK *pfn_notify)(cl_program program, void *user_data),
                                 void *user_data);

    static cl_kernel clCreateKernel(cl_program program,
                                    const char *kernel_name,
                                    cl_int *errcode_ret);

    static cl_int clEnqueueFillImage(cl_command_queue command_queue,
                                     cl_mem image,
                                     const void *fill_color,
                                     const size_t *origin,
                                     const size_t *region,
                                     cl_uint num_events_in_wait_list,
                                     const cl_event *event_wait_list,
                                     cl_event *event);

    static cl_int clSetKernelArg(cl_kernel kernel,
                                 cl_uint arg_index,
                                 size_t arg_size,
                                 const void *arg_value);

    static cl_int clEnqueueNDRangeKernel(cl_command_queue command_queue,
                                         cl_kernel kernel,
                                         cl_uint work_dim,
                                         const size_t *global_work_offset,
                                         const size_t *global_work_size,
                                         const size_t *local_work_size,
                                         cl_uint num_events_in_wait_list,
                                         const cl_event *event_wait_list,
                                         cl_event *event);

    static cl_int clReleaseDevice(cl_device_id device);

    static cl_int clReleaseContext(cl_context context);

    static cl_int clReleaseCommandQueue(cl_command_queue command_queue);

    static cl_int clReleaseMemObject(cl_mem memobj);

    static cl_int clReleaseProgram(cl_program program);

    static cl_int clReleaseKernel(cl_kernel kernel);
};
