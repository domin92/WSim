#pragma once

#include "Source/WSimSimulation/Simulation/Simulation.h"
#include "Tests/UnitTests/Simulation/OclTest.h"

struct SimulationTest : OclTest {
};

TEST_F(SimulationTest, givenDefaultParametersWhenCreatingSimulationThenItWorks) {
    Simulation simulation{0, 0, Vec3{10, 10, 10}};
}
