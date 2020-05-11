#pragma once

#include <cstdint>

struct Vec3 {
    Vec3() = default;
    Vec3(size_t x, size_t y, size_t z) : x(x), y(y), z(z) {}
    union {
        struct {
            size_t x, y, z;
        };
        size_t ptr[3];
    };
    size_t getRequiredBufferSize(size_t pixelSize) const {
        return x * y * z * pixelSize;
    }
    bool operator==(const Vec3 &o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator<=(const Vec3 &o) const { return x <= o.x && y <= o.y && z <= o.z; }
};
