#pragma once

#include "Source/WSimCore/Simulation/BorderMaths.h"
#include "Source/WSimCore/Utils/OpenCL.h"

class OclCopyHelper {
public:
    // clang-format off
    enum class Dim { X, Y, Z };
    enum class End { Lower, Higher };
    // clang-format on

    OclCopyHelper(PositionInGrid grid, cl_command_queue &commandQueue, size_t border, cl_mem image, OCL::Vec3 baseSize);

    void preShareCopySide(void *outputBuffer, Dim dimension, End end);
    void preShareCopyEdge(void *outputBuffer, Dim dimension1, End end1, Dim dimension2, End end2);
    void preShareCopyCorner(void *outputBuffer, End endX, End endY, End endZ);

    void postShareCopySide(const void *inputBuffer, Dim dimension, End end);
    void postShareCopyEdge(const void *inputBuffer, Dim dimension1, End end1, Dim dimension2, End end2);
    void postShareCopyCorner(const void *inputBuffer, End endX, End endY, End endZ);

private:
    bool shouldNotShare(Dim dimension, End end);
    void computePreShareCopyOffsetComponent(OCL::Vec3 &offset, Dim dimension, End end);
    void computePostShareCopyOffsetComponent(OCL::Vec3 &offset, Dim dimension, End end);
    static size_t &selectDimension(OCL::Vec3 &vector, Dim dimension);
    static const size_t &selectDimension(const OCL::Vec3 &vector, Dim dimension);

    const PositionInGrid grid;
    const cl_command_queue &commandQueue;
    const size_t border;
    const cl_mem image;
    const OCL::Vec3 baseSize;
};
