#include "Source/WSimCommon/EdgeHelpers.h"
#include "Source/WSimCommon/Error.h"

size_t &selectDimension(Vec3 &vector, Dim dimension) {
    const size_t &result = selectDimension(const_cast<const Vec3 &>(vector), dimension);
    return const_cast<size_t &>(result); // this is safe, because vector was not const in the first place
}

const size_t &selectDimension(const Vec3 &vector, Dim dimension) {
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
