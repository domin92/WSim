#include "Source/LevelSetPlayground/LevelSetHelper.h"
#include "Source/WSimRenderer/LevelSetRenderer3D.hpp"

#include <algorithm>
#include <iostream>

struct LevelSetRendererCallbacksImpl : LevelSetRendererCallbacks {
    LevelSetRendererCallbacksImpl(float *data) : data(data) {}
    void updateSimulation(float deltaTimeSeconds) override{};
    float *getData() override { return data; }

    float *const data;
};

int main() {
    const Vec3 size = {30, 30, 30};
    const float sphereRadius = static_cast<float>(size.x - 1) / 2;
    auto levelSet = std::make_unique<float[]>(size.getRequiredBufferSize(1));
    LevelSetHelper::initializeToSphere(levelSet.get(), size, sphereRadius);

    LevelSetRendererCallbacksImpl callbacks{levelSet.get()};
    LevelSetRenderer renderer{callbacks, 400, 400, size};
    renderer.mainLoop();
}
