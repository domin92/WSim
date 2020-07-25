#pragma once

#include "Source/WSimCommon/Vec3.hpp"

// clang-format off
enum class Dim { X, Y, Z };
enum class End { Lower, Higher };
// clang-format on

size_t &selectDimension(Vec3 &vector, Dim dimension);
const size_t &selectDimension(const Vec3 &vector, Dim dimension);
