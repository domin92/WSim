#pragma once

#include "Source/WSimCore/Utils/ImagePair.h"
#include "Source/WSimCore/Utils/OpenCL.h"

class Simulation;

// Each simulation step is allowed to decrease available border space. This is done in the case of
// sampling around a voxel - if we want to calculate NxNxN cube, while sampling one unit around each
// voxel, then we'll need (N+2)x(N+2)x(N+2) cube (that is border increase by 1) for correct results.
// Because we want the last step's output size to be equal to the actual simulation size (so we do not
// overcompute), simulation steps are created in reverse order.
class SimulationStep {
public:
    SimulationStep(Simulation &simulation, OCL::Vec3 &outputVelocitySize, OCL::Vec3 inputVelocitySize);
    virtual void run(float deltaTime) = 0;
    virtual void stop() = 0;

protected:
    Simulation &simulation;
    const OCL::Vec3 outputVelocitySize;
    const OCL::Vec3 inputVelocitySize;
};

// This is the first step (which is created lastly, because steps are created in reverse order).
// It performs border reduction from actual border being used to the size needed by next steps.
// The greater the reduction is, the greater velocities can be achieved without loss during
// the advection process.
class SimulationStepAdvection : public SimulationStep {
public:
    SimulationStepAdvection(Simulation &simulation, OCL::Vec3 &outputVelocitySize);
    void run(float deltaTime) override;
    void stop() override;

private:
    OCL::Kernel kernelAdvection;
};

// This step performs two things, resulting in total border decrease of 2
// 1. Calculate vorticity, which is the curl of velocity. This is a differential operator,
//    hence border decrease of 1
// 2. Change velocity according to vorticity. This involves calculating a gradient of vorticity
//    (another differential operator), so another border decrease of 1
class SimulationStepVorticityConfinement : public SimulationStep {
public:
    SimulationStepVorticityConfinement(Simulation &simulation, OCL::Vec3 &outputVelocitySize);
    void run(float deltaTime) override;
    void stop() override;

private:
    const OCL::Vec3 vorticitySize; // It is inputSize-1, because it is curl of velocity, which is a differential operator

    OCL::Kernel kernelCalculateVorticity;
    OCL::Kernel kernelApplyVorticityConfinement;
    OCL::Mem vorticity;
};

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
    void run(float deltaTime) override;
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
