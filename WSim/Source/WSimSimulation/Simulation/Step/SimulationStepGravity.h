#pragma once

#include "Source/WSimSimulation/Simulation/Step/SimulationStep.h"
#include "Source/WSimSimulation/Utils/OpenCL.h"

class SimulationStepGravity : public SimulationStep {
public:
    SimulationStepGravity(Simulation &simulation, Vec3 &outputVelocitySize);
    void run(float deltaTimeSeconds) override;
    void stop() override;

private:
    OCL::Kernel kernelApplyGravity;
};
