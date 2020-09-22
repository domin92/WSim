#include "Source/WSimSimulation/Simulation/BorderMaths.hpp"

#include <gtest/gtest.h>

TEST(PositionInGridTest, oneNodeInGrid) {
    Vec3 id{0, 0, 0};
    Vec3 size{1, 1, 1};
    PositionInGrid grid{id, size};
    EXPECT_TRUE(grid.edgeL);
    EXPECT_TRUE(grid.edgeR);
    EXPECT_TRUE(grid.edgeU);
    EXPECT_TRUE(grid.edgeD);
    EXPECT_TRUE(grid.edgeF);
    EXPECT_TRUE(grid.edgeB);
}

TEST(PositionInGridTest, nodeInCenter) {
    Vec3 id{1, 1, 1};
    Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_FALSE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_FALSE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, leftEdge) {
    Vec3 id{0, 1, 1};
    Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_TRUE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_FALSE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, downEdge) {
    Vec3 id{1, 0, 1};
    Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_FALSE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_TRUE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, frontEdge) {
    Vec3 id{1, 1, 0};
    Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_FALSE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_FALSE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_TRUE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, leftUpCorner) {
    Vec3 id{0, 0, 1};
    Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_TRUE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_TRUE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, leftUpFrontCorner) {
    Vec3 id{0, 0, 0};
    Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_TRUE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_TRUE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_TRUE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, rightDownBackCorner) {
    Vec3 id{2, 2, 2};
    Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_FALSE(grid.edgeL);
    EXPECT_TRUE(grid.edgeR);
    EXPECT_FALSE(grid.edgeU);
    EXPECT_TRUE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_TRUE(grid.edgeB);
}

TEST(BorderIncreaseDecreaseTest, givenAllBordersPresentWhenIncreasingAndDecreasingBorderThenAllDimensionsAreChangedTwice) {
    PositionInGrid grid{};
    const Vec3 size1{5, 5, 5};

    const Vec3 size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(15, size2.x);
    EXPECT_EQ(15, size2.y);
    EXPECT_EQ(15, size2.z);

    const Vec3 size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(9, size3.x);
    EXPECT_EQ(9, size3.y);
    EXPECT_EQ(9, size3.z);
}

TEST(BorderIncreaseDecreaseTest, givenNoBordersPresentWhenIncreasingAndDecreasingBorderThenAllDimensionsAreNotChanged) {
    PositionInGrid grid{};
    grid.edgeL = grid.edgeR = grid.edgeU = grid.edgeD = grid.edgeF = grid.edgeB = true;
    const Vec3 size1{5, 5, 5};

    const Vec3 size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(5, size2.x);
    EXPECT_EQ(5, size2.y);
    EXPECT_EQ(5, size2.z);

    const Vec3 size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(5, size3.x);
    EXPECT_EQ(5, size3.y);
    EXPECT_EQ(5, size3.z);
}

TEST(BorderIncreaseDecreaseTest, givenOneEdgeWhenIncreasingAndDecreasingBorderThenItsDimensionIsChangedOnlyOnce) {
    PositionInGrid grid{};
    const Vec3 size1{5, 5, 5};
    Vec3 size2, size3;

    // Left
    grid = {};
    grid.edgeL = true;
    size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(10, size2.x);
    EXPECT_EQ(15, size2.y);
    EXPECT_EQ(15, size2.z);
    size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(7, size3.x);
    EXPECT_EQ(9, size3.y);
    EXPECT_EQ(9, size3.z);

    // Right
    grid = {};
    grid.edgeR = true;
    size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(10, size2.x);
    EXPECT_EQ(15, size2.y);
    EXPECT_EQ(15, size2.z);
    size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(7, size3.x);
    EXPECT_EQ(9, size3.y);
    EXPECT_EQ(9, size3.z);

    // Up
    grid = {};
    grid.edgeU = true;
    size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(15, size2.x);
    EXPECT_EQ(10, size2.y);
    EXPECT_EQ(15, size2.z);
    size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(9, size3.x);
    EXPECT_EQ(7, size3.y);
    EXPECT_EQ(9, size3.z);

    // Down
    grid = {};
    grid.edgeD = true;
    size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(15, size2.x);
    EXPECT_EQ(10, size2.y);
    EXPECT_EQ(15, size2.z);
    size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(9, size3.x);
    EXPECT_EQ(7, size3.y);
    EXPECT_EQ(9, size3.z);

    // Front
    grid = {};
    grid.edgeF = true;
    size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(15, size2.x);
    EXPECT_EQ(15, size2.y);
    EXPECT_EQ(10, size2.z);
    size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(9, size3.x);
    EXPECT_EQ(9, size3.y);
    EXPECT_EQ(7, size3.z);

    // Back
    grid = {};
    grid.edgeB = true;
    size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(15, size2.x);
    EXPECT_EQ(15, size2.y);
    EXPECT_EQ(10, size2.z);
    size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(9, size3.x);
    EXPECT_EQ(9, size3.y);
    EXPECT_EQ(7, size3.z);
}

TEST(BorderIncreaseDecreaseTest, givenLeftDownFrontCornerWhenIncreasingAndDecreasingBorderThenAllDimensionsAreChangedOnce) {
    PositionInGrid grid{};
    grid.edgeL = true;
    grid.edgeD = true;
    grid.edgeF = true;
    const Vec3 size1{5, 5, 5};
    Vec3 size2, size3;

    size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(10, size2.x);
    EXPECT_EQ(10, size2.y);
    EXPECT_EQ(10, size2.z);
    size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(7, size3.x);
    EXPECT_EQ(7, size3.y);
    EXPECT_EQ(7, size3.z);
}

TEST(BorderIncreaseDecreaseTest, givenRightUpBackCornerWhenIncreasingAndDecreasingBorderThenAllDimensionsAreChangedOnce) {
    PositionInGrid grid{};
    grid.edgeR = true;
    grid.edgeU = true;
    grid.edgeB = true;
    const Vec3 size1{5, 5, 5};
    Vec3 size2, size3;

    size2 = increaseBorder(size1, grid, 5);
    EXPECT_EQ(10, size2.x);
    EXPECT_EQ(10, size2.y);
    EXPECT_EQ(10, size2.z);
    size3 = decreaseBorder(size2, grid, 3, size1);
    EXPECT_EQ(7, size3.x);
    EXPECT_EQ(7, size3.y);
    EXPECT_EQ(7, size3.z);
}

TEST(CalculateBorderOffsetTest, givenAllBordersPresentWhenCalculatingBorderOffsetThenApplyOffsetInAllDimensions) {
    PositionInGrid grid{};
    const Vec3 totalSize{20, 20, 20};
    const Vec3 usedSize{10, 10, 10};
    Vec3 offset;

    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(5, offset.z);
}

TEST(CalculateBorderOffsetTest, givenNoBordersPresentWhenCalculatingBorderOffsetThenApplyOffsetIsZero) {
    // This is kind of illegal case - when we have no borders, totalSize should be always equal to usedSize
    // Nevertheless, the function should work, ignoring overallocated size
    PositionInGrid grid{};
    grid.edgeL = grid.edgeR = grid.edgeU = grid.edgeD = grid.edgeF = grid.edgeB = true;
    const Vec3 totalSize{20, 20, 20};
    const Vec3 usedSize{10, 10, 10};
    Vec3 offset;

    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(0, offset.x);
    EXPECT_EQ(0, offset.y);
    EXPECT_EQ(0, offset.z);
}

TEST(CalculateBorderOffsetTest, givenLowerEdgeWhenCalculatingBorderOffsetThenItsDimensionIsZero) {
    PositionInGrid grid{};
    const Vec3 usedSize{10, 10, 10};
    Vec3 offset, totalSize;

    grid = {};
    grid.edgeL = true;
    totalSize = {15, 20, 20};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(0, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(5, offset.z);

    grid = {};
    grid.edgeU = true;
    totalSize = {20, 15, 20};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(0, offset.y);
    EXPECT_EQ(5, offset.z);

    grid = {};
    grid.edgeF = true;
    totalSize = {20, 20, 15};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(0, offset.z);
}

TEST(CalculateBorderOffsetTest, givenHigherEdgeWhenCalculatingBorderOffsetThenItsDimensionIsCorrect) {
    PositionInGrid grid{};
    const Vec3 usedSize{10, 10, 10};
    Vec3 offset, totalSize;

    grid = {};
    grid.edgeR = true;
    totalSize = {15, 20, 20};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(5, offset.z);

    grid = {};
    grid.edgeD = true;
    totalSize = {20, 15, 20};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(5, offset.z);

    grid = {};
    grid.edgeB = true;
    totalSize = {20, 20, 15};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(5, offset.z);
}

TEST(CalculateBorderOffsetTest, givenBothEdgesInSomeDimensionWhenCalculatingBorderOffsetThenItsOffsetIsZero) {
    PositionInGrid grid{};
    const Vec3 usedSize{10, 10, 10};
    Vec3 offset, totalSize;

    grid = {};
    grid.edgeL = true;
    grid.edgeR = true;
    totalSize = {10, 20, 20};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(0, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(5, offset.z);

    grid = {};
    grid.edgeD = true;
    grid.edgeU = true;
    totalSize = {20, 10, 20};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(0, offset.y);
    EXPECT_EQ(5, offset.z);

    grid = {};
    grid.edgeF = true;
    grid.edgeB = true;
    totalSize = {20, 20, 10};
    offset = calculateBorderOffset(totalSize, usedSize, grid);
    EXPECT_EQ(5, offset.x);
    EXPECT_EQ(5, offset.y);
    EXPECT_EQ(0, offset.z);
}

TEST(BorderMathsTest, givenLeftAndDownEdgesThenExampleScenarioIsCorrectlyProcessed) {
    PositionInGrid grid{};
    grid.edgeL = grid.edgeD = true;
    const Vec3 baseSize = {10, 10, 10};

    Vec3 allocatedSize = increaseBorder(baseSize, grid, 5);
    EXPECT_EQ(15, allocatedSize.x);
    EXPECT_EQ(15, allocatedSize.y);
    EXPECT_EQ(20, allocatedSize.z);

    Vec3 simulationSize = decreaseBorder(allocatedSize, grid, 1, baseSize);
    EXPECT_EQ(14, simulationSize.x);
    EXPECT_EQ(14, simulationSize.y);
    EXPECT_EQ(18, simulationSize.z);

    Vec3 offset = calculateBorderOffset(allocatedSize, simulationSize, grid);
    EXPECT_EQ(0, offset.x);
    EXPECT_EQ(1, offset.y);
    EXPECT_EQ(1, offset.z);
}

TEST(BorderMathsTest, givenLeftUpFrontCornerThenExampleScenarioIsCorrectlyProcessed) {
    PositionInGrid grid{};
    grid.edgeL = grid.edgeU = grid.edgeF = true;
    const Vec3 baseSize = { 10, 10, 10 };

    Vec3 allocatedSize = increaseBorder(baseSize, grid, 5);
    EXPECT_EQ(15, allocatedSize.x);
    EXPECT_EQ(15, allocatedSize.y);
    EXPECT_EQ(15, allocatedSize.z);

    Vec3 simulationSize = decreaseBorder(allocatedSize, grid, 1, baseSize);
    EXPECT_EQ(14, simulationSize.x);
    EXPECT_EQ(14, simulationSize.y);
    EXPECT_EQ(14, simulationSize.z);

    Vec3 offset = calculateBorderOffset(allocatedSize, simulationSize, grid);
    EXPECT_EQ(0, offset.x);
    EXPECT_EQ(0, offset.y);
    EXPECT_EQ(0, offset.z);
}

TEST(BorderMathsTest, givenRightDownBackCornerThenExampleScenarioIsCorrectlyProcessed) {
    PositionInGrid grid{};
    grid.edgeR = grid.edgeD = grid.edgeB = true;
    const Vec3 baseSize = { 10, 10, 10 };

    Vec3 allocatedSize = increaseBorder(baseSize, grid, 5);
    EXPECT_EQ(15, allocatedSize.x);
    EXPECT_EQ(15, allocatedSize.y);
    EXPECT_EQ(15, allocatedSize.z);

    Vec3 simulationSize = decreaseBorder(allocatedSize, grid, 1, baseSize);
    EXPECT_EQ(14, simulationSize.x);
    EXPECT_EQ(14, simulationSize.y);
    EXPECT_EQ(14, simulationSize.z);

    Vec3 offset = calculateBorderOffset(allocatedSize, simulationSize, grid);
    EXPECT_EQ(1, offset.x);
    EXPECT_EQ(1, offset.y);
    EXPECT_EQ(1, offset.z);
}
