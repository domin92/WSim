#pragma once

#include "Source/WSimSimulation/Simulation/Step/SimulationStep.h"
#include "Source/WSimSimulation/Utils/OpenCL.h"

// This is the first step (which is created lastly, because steps are created in reverse order).
// It performs border reduction from actual border being used to the size needed by next steps.
// The greater the reduction is, the greater velocities can be achieved without loss during
// the advection process.
class SimulationStepAdvection : public SimulationStep {
public:
    SimulationStepAdvection(Simulation &simulation, Vec3 &outputVelocitySize);
    void run(float deltaTimeSeconds) override;
    void stop() override;

private:
    OCL::Kernel kernelAdvection;
};
