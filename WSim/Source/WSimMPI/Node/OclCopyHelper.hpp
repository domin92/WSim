#pragma once

#include "Source/WSimCommon/EdgeHelpers.hpp"
#include "Source/WSimSimulation/Simulation/BorderMaths.hpp"
#include "Source/WSimSimulation/Utils/OpenCL.hpp"

class OclCopyHelper {
public:
    OclCopyHelper(PositionInGrid grid, cl_command_queue &commandQueue, size_t border, Vec3 baseSize);

    void preShareCopySide(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, void *outputBuffer, Dim dimension, End end);
    void preShareCopyEdge(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, void *outputBuffer, Dim dimension1, End end1, Dim dimension2, End end2);
    void preShareCopyCorner(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, void *outputBuffer, End endZ, End endY, End endX);

    void postShareCopySide(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, const void *inputBuffer, Dim dimension, End end);
    void postShareCopyEdge(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, const void *inputBuffer, Dim dimension1, End end1, Dim dimension2, End end2);
    void postShareCopyCorner(cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize, const void *inputBuffer, End endZ, End endY, End endX);

private:
    bool shouldNotShare(Dim dimension, End end);
    void computePreShareCopyOffsetComponent(Vec3 &offset, Dim dimension, End end);
    void computePostShareCopyOffsetComponent(Vec3 &offset, Dim dimension, End end);
    void decreaseOffsetIfNodeIsLowerEdge(Vec3& offset);

    const PositionInGrid grid;
    const cl_command_queue &commandQueue;
    const size_t border;
    const Vec3 baseSize;
};
