#include "Source/LevelSetPlayground/LevelSetHelper.h"
#include "Source/WSimRenderer/LevelSetRenderer3D.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

#include <algorithm>
#include <iostream>

struct LevelSetRendererCallbacksImpl : LevelSetRendererCallbacks {
    LevelSetRendererCallbacksImpl(Simulation &simulation, float *data)
        : simulation(simulation),
          data(data) {
        simulation.writeColor(data);
    }

    void updateSimulation(float deltaTimeSeconds) override {
        simulation.stepSimulation(deltaTimeSeconds);
    };

    float *getData() override {
        simulation.readColor(data);
        return data;
    }

    Simulation &simulation;
    float *const data;
};

int main() {
    const size_t s = 40;
    const Vec3 size = {s, s, s};
    const float sphereRadius = static_cast<float>(size.x - 1) / 2;
    auto levelSet = std::make_unique<float[]>(size.getRequiredBufferSize(1));
    LevelSetHelper::initializeToSphere(levelSet.get(), size, sphereRadius);

    Simulation simulation{0u, 0u, size, true};
    simulation.setGravityForce(0.1f);
    simulation.addObstacleAllWalls();

    LevelSetRendererCallbacksImpl callbacks{simulation, levelSet.get()};
    LevelSetRenderer renderer{callbacks, 400, 400, size};
    renderer.mainLoop();
}
