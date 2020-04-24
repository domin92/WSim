#include "OpenCLCopies.h"

#include "Source/WSimCore/Utils/Error.h"

OclCopyHelper::OclCopyHelper(PositionInGrid grid, cl_command_queue &commandQueue, size_t border, cl_mem image, OCL::Vec3 baseSize)
    : grid(grid),
      commandQueue(commandQueue),
      border(border),
      image(image),
      baseSize(baseSize) {}

void OclCopyHelper::preShareCopySide(void *outputBuffer, Dim dimension, End end) {
    if (shouldNotShare(dimension, end)) {
        return;
    }

    // Compute 3D offset
    OCL::Vec3 offset{border, border, border};
    computePreShareCopyOffsetComponent(offset, dimension, end);

    // Compute 3D size
    OCL::Vec3 size = baseSize;
    selectDimension(size, dimension) = border;

    // Perform actual read
    OCL::enqueueReadImage3D(commandQueue, image, CL_FALSE, offset, size, outputBuffer);
}

void OclCopyHelper::preShareCopyEdge(void *outputBuffer, Dim dimension1, End end1, Dim dimension2, End end2) {
    wsimErrorIf(dimension1 == dimension2);
    wsimErrorIf(end1 == end2);
    if (shouldNotShare(dimension1, end1) || shouldNotShare(dimension2, end2)) {
        return;
    }

    // Compute 3D offset
    OCL::Vec3 offset{border, border, border};
    computePreShareCopyOffsetComponent(offset, dimension1, end1);
    computePreShareCopyOffsetComponent(offset, dimension2, end2);

    // Compute 3D size
    OCL::Vec3 size = baseSize;
    selectDimension(size, dimension1) = border;
    selectDimension(size, dimension2) = border;

    // Perform actual read
    OCL::enqueueReadImage3D(commandQueue, image, CL_FALSE, offset, size, outputBuffer);
}

void OclCopyHelper::preShareCopyCorner(void *outputBuffer, End endX, End endY, End endZ) {
    if (shouldNotShare(Dim::X, endX) || shouldNotShare(Dim::Y, endY) || shouldNotShare(Dim::Z, endZ)) {
        return;
    }

    // Compute 3D offset
    OCL::Vec3 offset{};
    computePreShareCopyOffsetComponent(offset, Dim::X, endX);
    computePreShareCopyOffsetComponent(offset, Dim::Y, endY);
    computePreShareCopyOffsetComponent(offset, Dim::Z, endZ);

    // Compute 3D size
    OCL::Vec3 size{border, border, border};

    // Perform actual read
    OCL::enqueueReadImage3D(commandQueue, image, CL_FALSE, offset, size, outputBuffer);
}

void OclCopyHelper::postShareCopySide(const void *inputBuffer, Dim dimension, End end) {
    if (shouldNotShare(dimension, end)) {
        return;
    }

    // Compute 3D offset
    OCL::Vec3 offset{border, border, border};
    computePostShareCopyOffsetComponent(offset, dimension, end);

    // Compute 3D size
    OCL::Vec3 size = baseSize;
    selectDimension(size, dimension) = border;

    // Perform actual write
    OCL::enqueueWriteImage3D(commandQueue, image, CL_FALSE, offset, size, inputBuffer);
}

void OclCopyHelper::postShareCopyEdge(const void *inputBuffer, Dim dimension1, End end1, Dim dimension2, End end2) {
    wsimErrorIf(dimension1 == dimension2);
    wsimErrorIf(end1 == end2);
    if (shouldNotShare(dimension1, end1) || shouldNotShare(dimension2, end2)) {
        return;
    }

    // Compute 3D offset
    OCL::Vec3 offset{border, border, border};
    computePostShareCopyOffsetComponent(offset, dimension1, end1);
    computePostShareCopyOffsetComponent(offset, dimension2, end2);

    // Compute 3D size
    OCL::Vec3 size = baseSize;
    selectDimension(size, dimension1) = border;
    selectDimension(size, dimension2) = border;

    // Perform actual write
    OCL::enqueueWriteImage3D(commandQueue, image, CL_FALSE, offset, size, inputBuffer);
}

void OclCopyHelper::postShareCopyCorner(const void *inputBuffer, End endX, End endY, End endZ) {
    if (shouldNotShare(Dim::X, endX) || shouldNotShare(Dim::Y, endY) || shouldNotShare(Dim::Z, endZ)) {
        return;
    }

    // Compute 3D offset
    OCL::Vec3 offset{};
    computePostShareCopyOffsetComponent(offset, Dim::X, endX);
    computePostShareCopyOffsetComponent(offset, Dim::Y, endY);
    computePostShareCopyOffsetComponent(offset, Dim::Z, endZ);

    // Compute 3D size
    OCL::Vec3 size{border, border, border};

    // Perform actual write
    OCL::enqueueWriteImage3D(commandQueue, image, CL_FALSE, offset, size, inputBuffer);
}

bool OclCopyHelper::shouldNotShare(Dim dimension, End end) {
    if (end == End::Lower) {
        switch (dimension) {
        case Dim::X:
            return grid.edgeL;
        case Dim::Y:
            return grid.edgeD;
        case Dim::Z:
            return grid.edgeF;
        default:
            wsimError();
        }
    } else if (end == End::Higher) {
        switch (dimension) {
        case Dim::X:
            return grid.edgeR;
        case Dim::Y:
            return grid.edgeU;
        case Dim::Z:
            return grid.edgeB;
        default:
            wsimError();
        }
    } else {
        wsimError();
    }
}

void OclCopyHelper::computePreShareCopyOffsetComponent(OCL::Vec3 &offset, Dim dimension, End end) {
    if (end == End::Lower) {
        selectDimension(offset, dimension) = border;
    } else {
        selectDimension(offset, dimension) = selectDimension(baseSize, dimension);
    }
}

void OclCopyHelper::computePostShareCopyOffsetComponent(OCL::Vec3 &offset, Dim dimension, End end) {
    if (end == End::Lower) {
        selectDimension(offset, dimension) = 0;
    } else {
        selectDimension(offset, dimension) = selectDimension(baseSize, dimension) + border;
    }
}

size_t &OclCopyHelper::selectDimension(OCL::Vec3 &vector, Dim dimension) {
    const size_t &result = selectDimension(const_cast<const OCL::Vec3 &>(vector), dimension);
    return const_cast<size_t &>(result); // this is safe, because vector was not const in the first place
}

const size_t &OclCopyHelper::selectDimension(const OCL::Vec3 &vector, Dim dimension) {
    switch (dimension) {
    case Dim::X:
        return vector.x;
    case Dim::Y:
        return vector.y;
    case Dim::Z:
        return vector.z;
    default:
        wsimError();
    }
}