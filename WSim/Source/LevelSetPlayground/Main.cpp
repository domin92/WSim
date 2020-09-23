#include "Source/LevelSetPlayground/LevelSetHelper.h"

#include <algorithm>
#include <iostream>

int main() {
    const Vec3 size = {11, 11, 1};
    const float sphereRadius = static_cast<float>(size.x - 1) / 2;
    auto levelSet = std::make_unique<float[]>(size.getRequiredBufferSize(1));
    LevelSetHelper::initializeToSphere(levelSet.get(), size, sphereRadius);
}
