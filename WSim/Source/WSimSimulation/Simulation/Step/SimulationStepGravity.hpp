#pragma once

#include "Source/WSimSimulation/Simulation/Step/SimulationStep.hpp"
#include "Source/WSimSimulation/Utils/OpenCL.hpp"

class SimulationStepGravity : public SimulationStep {
public:
    SimulationStepGravity(Simulation &simulation, Vec3 &outputVelocitySize);
    void run(float deltaTimeSeconds) override;
    void stop() override;

private:
    OCL::Kernel kernelApplyGravity;
};
