#include "SimulationStep.hpp"

#include "Source/WSimCommon/Error.hpp"

SimulationStep::SimulationStep(Simulation &simulation, Vec3 &outputVelocitySize, Vec3 inputVelocitySize)
    : SimulationStep(simulation, const_cast<const Vec3 &>(outputVelocitySize), inputVelocitySize) {
    // Input size of this step, will be use as an output size of previous steps, which will be constructed later
    // (steps are constructed from last to first)
    outputVelocitySize = inputVelocitySize;
}

SimulationStep::SimulationStep(Simulation &simulation, const Vec3 &outputVelocitySize, Vec3 inputVelocitySize)
    : simulation(simulation),
      outputVelocitySize(outputVelocitySize),
      inputVelocitySize(inputVelocitySize) {
    // Simulation size can't be enlarged by the simulation step
    wsimErrorUnless(outputVelocitySize <= inputVelocitySize);
}
