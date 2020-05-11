#pragma once

#include "Source/WSimSimulation/Simulation/Step/SimulationStep.h"
#include "Source/WSimSimulation/Utils/ImagePair.h"

// This step performs three things, resulting in total border decrease of jacobiIterations+1:
// 1. Calculate velocity divergence. This decreases border by 1
// 2. Calculate pressure iteratively with Jacobi method. First iteration can be done without
//    decreasing border, but every subsequent iterations decrease it by 1. So border decreases
//    by n-1 in total.
// 3. Apply calculated pressure to the velocity field. This involves calculating pressure gradient
//    so again border is decreased by 1.
class SimulationStepPressure : public SimulationStep {
public:
    SimulationStepPressure(Simulation &simulation, size_t jacobiIterations, OCL::Vec3 &outputVelocitySize);
    void run(float deltaTimeSeconds) override;
    void stop() override;

private:
    const size_t jacobiIterations;
    const OCL::Vec3 divergenceSize; // also the size of pressure field. It is inputSize-1, because divergence is a differential operator

    OCL::Kernel kernelDivergence;
    OCL::Kernel kernelPressureJacobi;
    OCL::Kernel kernelProjectVelocityToDivergenceFree;

    OCL::Mem divergence;
    Image3DPair pressure;
};
