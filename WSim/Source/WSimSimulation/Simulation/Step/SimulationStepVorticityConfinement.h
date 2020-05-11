#pragma once

#include "Source/WSimSimulation/Simulation/Step/SimulationStep.h"

// This step performs two things, resulting in total border decrease of 2
// 1. Calculate vorticity, which is the curl of velocity. This is a differential operator,
//    hence border decrease of 1
// 2. Change velocity according to vorticity. This involves calculating a gradient of vorticity
//    (another differential operator), so another border decrease of 1
class SimulationStepVorticityConfinement : public SimulationStep {
public:
    SimulationStepVorticityConfinement(Simulation &simulation, OCL::Vec3 &outputVelocitySize);
    void run(float deltaTimeSeconds) override;
    void stop() override;

private:
    const OCL::Vec3 vorticitySize; // It is inputSize-1, because it is curl of velocity, which is a differential operator

    OCL::Kernel kernelCalculateVorticity;
    OCL::Kernel kernelApplyVorticityConfinement;
    OCL::Mem vorticity;
};
