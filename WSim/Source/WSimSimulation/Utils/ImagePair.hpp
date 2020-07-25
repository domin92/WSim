#pragma once

#include "Source/WSimSimulation/Utils/OpenCL.hpp"

struct Image3DPair {
    explicit Image3DPair(cl_context context, Vec3 size, const cl_image_format &format) {
        images[0] = OCL::createReadWriteImage3D(context, size, format);
        images[1] = OCL::createReadWriteImage3D(context, size, format);
    }

    OCL::Mem &getSource() { return images[sourceResourceIndex]; }
    OCL::Mem &getDestination() { return images[sourceResourceIndex^1]; }
    OCL::Mem &getDestinationAndSwap() {
        swap();
        return getSource();
    }
    void swap() { sourceResourceIndex ^= 1; }

protected:
    int sourceResourceIndex = 0;
    OCL::Mem images[2] = {};
};
