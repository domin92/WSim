#include "OclCopyHelper.hpp"

#include "Source/WSimCommon/Error.hpp"

#include <type_traits>

template <typename A>
constexpr inline A ptrAdd(A ptr, size_t offset) {
    static_assert(std::is_pointer_v<A>);
    return reinterpret_cast<A>(reinterpret_cast<uintptr_t>(ptr) + static_cast<uintptr_t>(offset));
}

OclCopyHelper::OclCopyHelper(PositionInGrid grid, cl_command_queue &commandQueue, size_t border, Vec3 baseSize)
    : grid(grid),
      commandQueue(commandQueue),
      border(border),
      baseSize(baseSize) {}

void OclCopyHelper::preShareCopySide(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, void *outputBuffer, Dim dimension, End end) {
    if (shouldNotShare(dimension, end)) {
        return;
    }

    // Compute 3D offset
    Vec3 offset{border, border, border};
    computePreShareCopyOffsetComponent(offset, dimension, end);
    decreaseOffsetIfNodeIsLowerEdge(offset);

    // Compute 3D size
    Vec3 size = baseSize;
    selectDimension(size, dimension) = border;

    // Perform actual read
    const auto readAddress = ptrAdd(outputBuffer, numberOfImagesToSkip * size.getRequiredBufferSize(skipPixelSize));
    OCL::enqueueReadImage3D(commandQueue, image, CL_FALSE, offset, size, readAddress);
}

void OclCopyHelper::preShareCopyEdge(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, void *outputBuffer, Dim dimension1, End end1, Dim dimension2, End end2) {
    wsimErrorIf(dimension1 == dimension2);
    if (shouldNotShare(dimension1, end1) || shouldNotShare(dimension2, end2)) {
        return;
    }

    // Compute 3D offset
    Vec3 offset{border, border, border};
    computePreShareCopyOffsetComponent(offset, dimension1, end1);
    computePreShareCopyOffsetComponent(offset, dimension2, end2);
    decreaseOffsetIfNodeIsLowerEdge(offset);

    // Compute 3D size
    Vec3 size = baseSize;
    selectDimension(size, dimension1) = border;
    selectDimension(size, dimension2) = border;

    // Perform actual read
    const auto readAddress = ptrAdd(outputBuffer, numberOfImagesToSkip * size.getRequiredBufferSize(skipPixelSize));
    OCL::enqueueReadImage3D(commandQueue, image, CL_FALSE, offset, size, readAddress);
}

void OclCopyHelper::preShareCopyCorner(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, void *outputBuffer, End endX, End endY, End endZ) {
    if (shouldNotShare(Dim::X, endX) || shouldNotShare(Dim::Y, endY) || shouldNotShare(Dim::Z, endZ)) {
        return;
    }

    // Compute 3D offset
    Vec3 offset{};
    computePreShareCopyOffsetComponent(offset, Dim::X, endX);
    computePreShareCopyOffsetComponent(offset, Dim::Y, endY);
    computePreShareCopyOffsetComponent(offset, Dim::Z, endZ);
    decreaseOffsetIfNodeIsLowerEdge(offset);

    // Compute 3D size
    Vec3 size{border, border, border};

    // Perform actual read
    const auto readAddress = ptrAdd(outputBuffer, numberOfImagesToSkip * size.getRequiredBufferSize(skipPixelSize));
    OCL::enqueueReadImage3D(commandQueue, image, CL_FALSE, offset, size, readAddress);
}

void OclCopyHelper::postShareCopySide(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, const void *inputBuffer, Dim dimension, End end) {
    if (shouldNotShare(dimension, end)) {
        return;
    }

    // Compute 3D offset
    Vec3 offset{border, border, border};
    computePostShareCopyOffsetComponent(offset, dimension, end);
    decreaseOffsetIfNodeIsLowerEdge(offset);

    // Compute 3D size
    Vec3 size = baseSize;
    selectDimension(size, dimension) = border;

    // Perform actual write
    const auto writeAddress = ptrAdd(inputBuffer, numberOfImagesToSkip * size.getRequiredBufferSize(skipPixelSize));
    OCL::enqueueWriteImage3D(commandQueue, image, CL_FALSE, offset, size, writeAddress);
}

void OclCopyHelper::postShareCopyEdge(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, const void *inputBuffer, Dim dimension1, End end1, Dim dimension2, End end2) {
    wsimErrorIf(dimension1 == dimension2);
    if (shouldNotShare(dimension1, end1) || shouldNotShare(dimension2, end2)) {
        return;
    }

    // Compute 3D offset
    Vec3 offset{border, border, border};
    computePostShareCopyOffsetComponent(offset, dimension1, end1);
    computePostShareCopyOffsetComponent(offset, dimension2, end2);
    decreaseOffsetIfNodeIsLowerEdge(offset);

    // Compute 3D size
    Vec3 size = baseSize;
    selectDimension(size, dimension1) = border;
    selectDimension(size, dimension2) = border;

    // Perform actual write
    const auto writeAddress = ptrAdd(inputBuffer, numberOfImagesToSkip * size.getRequiredBufferSize(skipPixelSize));
    OCL::enqueueWriteImage3D(commandQueue, image, CL_FALSE, offset, size, writeAddress);
}

void OclCopyHelper::postShareCopyCorner(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, const void *inputBuffer, End endX, End endY, End endZ) {
    if (shouldNotShare(Dim::X, endX) || shouldNotShare(Dim::Y, endY) || shouldNotShare(Dim::Z, endZ)) {
        return;
    }

    // Compute 3D offset
    Vec3 offset{};
    computePostShareCopyOffsetComponent(offset, Dim::X, endX);
    computePostShareCopyOffsetComponent(offset, Dim::Y, endY);
    computePostShareCopyOffsetComponent(offset, Dim::Z, endZ);
    decreaseOffsetIfNodeIsLowerEdge(offset);

    // Compute 3D size
    Vec3 size{border, border, border};

    // Perform actual write
    const auto writeAddress = ptrAdd(inputBuffer, numberOfImagesToSkip * size.getRequiredBufferSize(skipPixelSize));
    OCL::enqueueWriteImage3D(commandQueue, image, CL_FALSE, offset, size, writeAddress);
}

bool OclCopyHelper::shouldNotShare(Dim dimension, End end) {
    if (end == End::Lower) {
        switch (dimension) {
        case Dim::X:
            return grid.edgeL;
        case Dim::Y:
            return grid.edgeU;
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
            return grid.edgeD;
        case Dim::Z:
            return grid.edgeB;
        default:
            wsimError();
        }
    } else {
        wsimError();
    }
}

void OclCopyHelper::computePreShareCopyOffsetComponent(Vec3 &offset, Dim dimension, End end) {
    size_t &output = selectDimension(offset, dimension);
    if (end == End::Lower) {
        output = border;
    } else {
        output = selectDimension(baseSize, dimension);
    }
}

void OclCopyHelper::computePostShareCopyOffsetComponent(Vec3 &offset, Dim dimension, End end) {
    size_t &output = selectDimension(offset, dimension);
    if (end == End::Lower) {
        output = 0;
    } else {
        output = selectDimension(baseSize, dimension) + border;
    }
}

void OclCopyHelper::decreaseOffsetIfNodeIsLowerEdge(Vec3 &offset) {
    if (grid.edgeL) {
        wsimErrorIf(offset.x == 0);
        offset.x -= border;
    }
    if (grid.edgeU) {
        wsimErrorIf(offset.y == 0);
        offset.y -= border;
    }
    if (grid.edgeF) {
        wsimErrorIf(offset.z == 0);
        offset.z -= border;
    }
}
