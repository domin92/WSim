#pragma once

#include "Source/WSimRenderer/LevelSetRenderer3D.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

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
