#pragma once

#include "Source/WSimCore/Utils/OpenCL.h"

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
