#include "Simulation/Simulation.h"

#include <gtest/gtest.h>

static const PositionInGrid positionInGridAllBorders = {OCL::Vec3{1, 1, 1}, OCL::Vec3{3, 3, 3}};
static const PositionInGrid positionInGridLeftEdge = {OCL::Vec3{0, 1, 1}, OCL::Vec3{3, 3, 3}};
static const PositionInGrid positionInGridLeftUpFrontEdge = {OCL::Vec3{0, 0, 0}, OCL::Vec3{3, 3, 3}};
static const PositionInGrid positionInGridLeftUpBackEdge = {OCL::Vec3{0, 0, 2}, OCL::Vec3{3, 3, 3}};
static const PositionInGrid positionInGridTheOnlyNode = {OCL::Vec3{0, 0, 0}, OCL::Vec3{1, 1, 1}};

struct MockSimulation : Simulation {
    using Simulation::calculateBorderOffset;
    using Simulation::calculateSimulationSizeWithBorder;
};

TEST(SimulationInitialCalculationsTest, givenAllBordersPresentWhenCalculatingBorderOffsetThenApplyToAllDimensions) {
    OCL::Vec3 offset = MockSimulation::calculateBorderOffset(positionInGridAllBorders, 5);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(5, offset.z);
}

TEST(SimulationInitialCalculationsTest, givenNoLeftBorderPresentWhenCalculatingBorderOffsetThenApplyToDimensionsXY) {
    OCL::Vec3 offset = MockSimulation::calculateBorderOffset(positionInGridLeftEdge, 5);
    EXPECT_EQ(0, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(5, offset.z);
}

TEST(SimulationInitialCalculationsTest, givenNoLowerBordersPresentWhenCalculatingBorderOffsetThenDoNotApply) {
    OCL::Vec3 offset = MockSimulation::calculateBorderOffset(positionInGridLeftUpFrontEdge, 5);
    EXPECT_EQ(0, offset.x);
    EXPECT_EQ(0, offset.y);
    EXPECT_EQ(0, offset.z);
}

TEST(SimulationInitialCalculationsTest, givenOnlyZBorderPresentWhenCalculatingBorderOffsetThenApplyToDimensionZ) {
    OCL::Vec3 offset = MockSimulation::calculateBorderOffset(positionInGridLeftUpBackEdge, 5);
    EXPECT_EQ(0, offset.x);
    EXPECT_EQ(0, offset.y);
    EXPECT_EQ(5, offset.z);
}

TEST(SimulationInitialCalculationsTest, givenAllBordersPresentWhenCalculatingSimulationSizeWithBorderThenApplyToAllDimensions) {
    OCL::Vec3 simulationSize{100, 100, 100};
    OCL::Vec3 size = MockSimulation::calculateSimulationSizeWithBorder(simulationSize, positionInGridAllBorders, 5);
    EXPECT_EQ(110, size.x);
    EXPECT_EQ(110, size.y);
    EXPECT_EQ(110, size.z);
}

TEST(SimulationInitialCalculationsTest, givenNoLeftBorderPresentWhenCalculatingSimulationSizeWithBorderThenHorizontalSizeIsDecreased) {
    OCL::Vec3 simulationSize{100, 100, 100};
    OCL::Vec3 size = MockSimulation::calculateSimulationSizeWithBorder(simulationSize, positionInGridLeftEdge, 5);
    EXPECT_EQ(105, size.x);
    EXPECT_EQ(110, size.y);
    EXPECT_EQ(110, size.z);
}

TEST(SimulationInitialCalculationsTest, givenOneBorderInEachDimensionPresentWhenCalculatingSimulationSizeWithBorderThenApplyToEachDimensionOnce) {
    OCL::Vec3 simulationSize{100, 100, 100};
    OCL::Vec3 size = MockSimulation::calculateSimulationSizeWithBorder(simulationSize, positionInGridLeftUpFrontEdge, 5);
    EXPECT_EQ(105, size.x);
    EXPECT_EQ(105, size.y);
    EXPECT_EQ(105, size.z);
    size = MockSimulation::calculateSimulationSizeWithBorder(simulationSize, positionInGridLeftUpBackEdge, 5);
    EXPECT_EQ(105, size.x);
    EXPECT_EQ(105, size.y);
    EXPECT_EQ(105, size.z);
}

TEST(SimulationInitialCalculationsTest, givenNoBordersPresentWhenCalculatingSimulationSizeWithBorderThenDoNotApplyAtAll) {
    OCL::Vec3 simulationSize{100, 100, 100};
    OCL::Vec3 size = MockSimulation::calculateSimulationSizeWithBorder(simulationSize, positionInGridTheOnlyNode, 5);
    EXPECT_EQ(100, size.x);
    EXPECT_EQ(100, size.y);
    EXPECT_EQ(100, size.z);
}
