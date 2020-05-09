#include "Source/WSimCommon/EdgeHelpers.h"

size_t &selectDimension(OCL::Vec3 &vector, Dim dimension) {
    const size_t &result = selectDimension(const_cast<const OCL::Vec3 &>(vector), dimension);
    return const_cast<size_t &>(result); // this is safe, because vector was not const in the first place
}

const size_t &selectDimension(const OCL::Vec3 &vector, Dim dimension) {
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
