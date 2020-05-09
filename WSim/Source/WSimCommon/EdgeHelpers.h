#pragma once

#include "Source/WSimSimulation/Utils/OpenCL.h"

// clang-format off
enum class Dim { X, Y, Z };
enum class End { Lower, Higher };
// clang-format on

size_t &selectDimension(OCL::Vec3 &vector, Dim dimension);
const size_t &selectDimension(const OCL::Vec3 &vector, Dim dimension);
