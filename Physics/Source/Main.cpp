#include "Utils/OpenCL.h"

#include <memory>

struct Image3DPair {
    explicit Image3DPair(cl_context context, OCL::Vec3 size, const cl_image_format &format) {
        images[0] = OCL::createReadWriteImage3D(context, size, format);
        images[1] = OCL::createReadWriteImage3D(context, size, format);
    }

    OCL::Mem &getSource() { return images[sourceResourceIndex]; }
    OCL::Mem &getDestination() { return images[1 - sourceResourceIndex]; }
    void swap() { sourceResourceIndex = 1 - sourceResourceIndex; }

protected:
    int sourceResourceIndex = 0;
    OCL::Mem images[2] = {};
};

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
    OCL::Program divergenceProgram = OCL::createProgram(device, context, "pressure/divergence.cl", true);
    OCL::Program jacobiProgram = OCL::createProgram(device, context, "pressure/jacobi.cl", true);
    OCL::Program applyPressureProgram = OCL::createProgram(device, context, "pressure/apply_pressure.cl", true);
    OCL::Kernel calculateDivergence = OCL::createKernel(divergenceProgram, "calculate_divergence");
    OCL::Kernel jacobi = OCL::createKernel(jacobiProgram, "solve_jacobi_iteration");
    OCL::Kernel applyPressure = OCL::createKernel(applyPressureProgram, "apply_pressure");

    // Prepare image data
    OCL::Vec3 imageSize{100, 100, 1};
    cl_image_format vectorFieldFormat;
    vectorFieldFormat.image_channel_order = CL_RGBA;
    vectorFieldFormat.image_channel_data_type = CL_FLOAT;
    cl_image_format scalarFieldFormat;
    scalarFieldFormat.image_channel_order = CL_R;
    scalarFieldFormat.image_channel_data_type = CL_FLOAT;
    const auto vectorFieldPixelSize = sizeof(float) * 4;
    const auto scalarFieldPixelSize = sizeof(float);

    // Create images
    Image3DPair velocity{context, imageSize, vectorFieldFormat};
    Image3DPair divergence{context, imageSize, scalarFieldFormat};
    Image3DPair pressure{context, imageSize, scalarFieldFormat};
    Image3DPair color{context, imageSize, vectorFieldFormat};

    // Fill velocity buffer
    OCL::setKernelArgMem(fill, 0, velocity.getDestination());
    OCL::enqueueKernel3D(commandQueue, fill, imageSize);
    velocity.swap();

    // Advect velocity
    OCL::setKernelArgMem(advection, 0, velocity.getSource());      // inField
    OCL::setKernelArgMem(advection, 1, velocity.getSource());      // inVelocity
    OCL::setKernelArgFlt(advection, 2, 1.f);                       // inDeltaTime
    OCL::setKernelArgFlt(advection, 3, 1.f);                       // inDissipation
    OCL::setKernelArgMem(advection, 4, velocity.getDestination()); // outField
    OCL::enqueueKernel3D(commandQueue, advection, imageSize);
    velocity.swap();

    // Calculate divergence
    OCL::setKernelArgMem(calculateDivergence, 0, velocity.getSource());        // inVelocity
    OCL::setKernelArgMem(calculateDivergence, 1, divergence.getDestination()); // outDivergence
    OCL::enqueueKernel3D(commandQueue, calculateDivergence, imageSize);
    divergence.swap();

    // Calculate pressure
    OCL::setKernelArgMem(jacobi, 0, divergence.getSource());        // inDivergence
    for (int i = 0; i < 10; i++) {                                  //
        OCL::setKernelArgMem(jacobi, 1, pressure.getSource());      // inPressure
        OCL::setKernelArgMem(jacobi, 2, pressure.getDestination()); // outPressure
        OCL::enqueueKernel3D(commandQueue, jacobi, imageSize);
        pressure.swap();
    }

    // Apply pressure
    OCL::setKernelArgMem(applyPressure, 0, velocity.getSource());      // inVelocity
    OCL::setKernelArgMem(applyPressure, 1, pressure.getSource());      // inPressure
    OCL::setKernelArgMem(applyPressure, 2, velocity.getDestination()); // outVelocity
    OCL::enqueueKernel3D(commandQueue, applyPressure, imageSize);
    velocity.swap();

    // Advect Color
    OCL::setKernelArgMem(advection, 0, color.getSource());      // inField
    OCL::setKernelArgMem(advection, 1, velocity.getSource());   // inVelocity
    OCL::setKernelArgFlt(advection, 2, 1.f);                    // inDeltaTime
    OCL::setKernelArgFlt(advection, 3, 1.f);                    // inDissipation
    OCL::setKernelArgMem(advection, 4, color.getDestination()); // outField
    OCL::enqueueKernel3D(commandQueue, advection, imageSize);
    color.swap();

    int a = 0;
}
