#pragma once

#include "Source/WSimCommon/Vec3.h"

class Simulation;

// Each simulation step is allowed to decrease available border space. This is done in the case of
// sampling around a voxel - if we want to calculate NxNxN cube, while sampling one unit around each
// voxel, then we'll need (N+2)x(N+2)x(N+2) cube (that is border increase by 1) for correct results.
// Because we want the last step's output size to be equal to the actual simulation size (so we do not
// overcompute), simulation steps are created in reverse order.
class SimulationStep {
public:
    SimulationStep(Simulation &simulation, Vec3 &outputVelocitySize, Vec3 inputVelocitySize);
    virtual void run(float deltaTimeSeconds) = 0;
    virtual void stop() = 0;

protected:
    Simulation &simulation;
    const Vec3 outputVelocitySize;
    const Vec3 inputVelocitySize;
};
