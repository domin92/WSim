#pragma once

#include "Source/WSimSimulation/Simulation/Step/SimulationStep.h"
#include "Source/WSimSimulation/Utils/OpenCL.h"

// This is the last step of whole iteration, when simulation produces a color texture
// It advects RGBA color through the velocity field to achieve visible fluid movement.
// Color texture is extended texture (can have border area from neighbouring simulations)
// which is an input to this simulation step. It outputs color texture without border space
class SimulationStepColorAdvection : public SimulationStep {
public:
    SimulationStepColorAdvection(Simulation& simulation);
    void run(float deltaTimeSeconds) override;
    void stop() override;

private:
    OCL::Kernel kernelColorAdvection;
};
