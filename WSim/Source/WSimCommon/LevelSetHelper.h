#pragma once

#include "Source/WSimCommon/Vec3.hpp"

struct LevelSetHelper {

    static void initializeToSphere(float *levelSet, Vec3 levelSetSize, float sphereRadius);
    static void initializeToSphere(float *levelSet, Vec3 levelSetSize, FloatVec3 sphereOrigin, float sphereRadius);
    static void reinitializeLevelSet(float *levelSet, size_t size);
};

namespace IndexHelper {
template <typename T>
constexpr inline T idx(T x, T y, T z, Vec3 size) {
    return size.y * size.x * z + size.x * y + x;
}

template <typename T>
constexpr inline T idx(T x, T y, T size) {
    return size * y + x;
}

template <typename T>
constexpr inline T getX(T idx, T size) {
    return idx % size;
}

template <typename T>
constexpr inline T getY(T idx, T size) {
    return idx / size;
}
} // namespace IndexHelper
