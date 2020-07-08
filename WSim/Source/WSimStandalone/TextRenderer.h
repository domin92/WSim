#pragma once

#include "Source/WSimCommon/MainLooper.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"

class TextRenderer : public MainLooper {
public:
    TextRenderer(Simulation &simulation) : MainLooper(std::chrono::seconds(0)),
                                           simulation(simulation) {}

protected:
    void update(float deltaTime) override {
        simulation.stepSimulation(deltaTime);
        OCL::finish(simulation.getCommandQueue());
    }

private:
    Simulation &simulation;
};