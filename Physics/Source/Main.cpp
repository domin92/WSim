#include "Utils/OpenCL.h"

#include <memory>

int main() {
    // Setup
    cl_platform_id platform = OCL::createPlatform();
    OCL::Device device = OCL::createDevice(platform);
    OCL::Context context = OCL::createContext(platform, device);
    OCL::CommandQueue commandQueue = OCL::createCommandQueue(context, device);

    // Load kernels
    OCL::Program advectionProgram = OCL::createProgram(device, context, "advection.cl", true);
    OCL::Kernel advection = OCL::createKernel(advectionProgram, "advection3f");
    OCL::Program fillProgram = OCL::createProgram(device, context, "fill.cl", true);
    OCL::Kernel fill = OCL::createKernel(fillProgram, "fillVelocity");

    OCL::Vec3 imageSize{4, 4, 1};

    // Create image
    cl_image_format format;
    format.image_channel_order = CL_RGBA;
    format.image_channel_data_type = CL_FLOAT;
    const auto pixelSize = sizeof(float) * 4;
    OCL::Mem image = OCL::createReadWriteImage3D(context, imageSize, format);
    OCL::Mem outImage = OCL::createReadWriteImage3D(context, imageSize, format);

    // Fill velocity buffer
    OCL::setKernelArg(fill, 0, image);
    OCL::enqueueKernel3D(commandQueue, fill, imageSize);

    // Apply advection
    OCL::setKernelArg(advection, 0, image);
    OCL::setKernelArg(advection, 1, outImage);
    OCL::setKernelArg(advection, 2, image);
    OCL::setKernelArgFloat(advection, 3, 1.f);
    OCL::setKernelArgFloat(advection, 4, 1.f);
    OCL::enqueueKernel3D(commandQueue, advection, imageSize);

    // Read result
    auto buff = std::make_unique<char[]>(imageSize.getRequiredBufferSize(pixelSize));
    OCL::enqueueReadImage3D(commandQueue, image, CL_TRUE, imageSize, 0, 0, buff.get());
    OCL::enqueueReadImage3D(commandQueue, outImage, CL_TRUE, imageSize, 0, 0, buff.get());

    int a = 0;
}
