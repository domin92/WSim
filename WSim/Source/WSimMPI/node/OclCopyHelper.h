#pragma once

#include "Source/WSimSimulation/Simulation/BorderMaths.h"
#include "Source/WSimSimulation/Utils/OpenCL.h"

class OclCopyHelper {
public:
    // clang-format off
    enum class Dim { X, Y, Z };
    enum class End { Lower, Higher };
    // clang-format on

    OclCopyHelper(PositionInGrid grid, cl_command_queue &commandQueue, size_t border, OCL::Vec3 baseSize);

    void preShareCopySide(cl_mem image, size_t indexInOutputBuffer, void *outputBuffer, Dim dimension, End end);
    void preShareCopyEdge(cl_mem image, size_t indexInOutputBuffer, void *outputBuffer, Dim dimension1, End end1, Dim dimension2, End end2);
    void preShareCopyCorner(cl_mem image, size_t indexInOutputBuffer, void *outputBuffer, End endX, End endY, End endZ);

    void postShareCopySide(cl_mem image, size_t indexInOutputBuffer, const void *inputBuffer, Dim dimension, End end);
    void postShareCopyEdge(cl_mem image, size_t indexInOutputBuffer, const void *inputBuffer, Dim dimension1, End end1, Dim dimension2, End end2);
    void postShareCopyCorner(cl_mem image, size_t indexInOutputBuffer, const void *inputBuffer, End endX, End endY, End endZ);

private:
    bool shouldNotShare(Dim dimension, End end);
    void computePreShareCopyOffsetComponent(OCL::Vec3 &offset, Dim dimension, End end);
    void computePostShareCopyOffsetComponent(OCL::Vec3 &offset, Dim dimension, End end);
    bool isLowerBorderPresent(Dim dimension);
    static size_t &selectDimension(OCL::Vec3 &vector, Dim dimension);
    static const size_t &selectDimension(const OCL::Vec3 &vector, Dim dimension);

    const PositionInGrid grid;
    const cl_command_queue &commandQueue;
    const size_t border;
    const OCL::Vec3 baseSize;
};
