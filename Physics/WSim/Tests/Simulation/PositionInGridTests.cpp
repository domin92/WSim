#include "Source/WSimCore/Simulation/PositionInGrid.h"

#include <gtest/gtest.h>

TEST(PositionInGridTest, oneNodeInGrid) {
    OCL::Vec3 id{0, 0, 0};
    OCL::Vec3 size{1, 1, 1};
    PositionInGrid grid{id, size};
    EXPECT_TRUE(grid.edgeL);
    EXPECT_TRUE(grid.edgeR);
    EXPECT_TRUE(grid.edgeU);
    EXPECT_TRUE(grid.edgeD);
    EXPECT_TRUE(grid.edgeF);
    EXPECT_TRUE(grid.edgeB);
}

TEST(PositionInGridTest, nodeInCenter) {
    OCL::Vec3 id{1, 1, 1};
    OCL::Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_FALSE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_FALSE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, leftEdge) {
    OCL::Vec3 id{0, 1, 1};
    OCL::Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_TRUE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_FALSE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, leftDownCorner) {
    OCL::Vec3 id{0, 0, 1};
    OCL::Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_TRUE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_FALSE(grid.edgeU);
    EXPECT_TRUE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, leftDownBackCorner) {
    OCL::Vec3 id{0, 0, 0};
    OCL::Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_TRUE(grid.edgeL);
    EXPECT_FALSE(grid.edgeR);
    EXPECT_FALSE(grid.edgeU);
    EXPECT_TRUE(grid.edgeD);
    EXPECT_TRUE(grid.edgeF);
    EXPECT_FALSE(grid.edgeB);
}

TEST(PositionInGridTest, rightUpFrontCorner) {
    OCL::Vec3 id{2, 2, 2};
    OCL::Vec3 size{3, 3, 3};
    PositionInGrid grid{id, size};
    EXPECT_FALSE(grid.edgeL);
    EXPECT_TRUE(grid.edgeR);
    EXPECT_TRUE(grid.edgeU);
    EXPECT_FALSE(grid.edgeD);
    EXPECT_FALSE(grid.edgeF);
    EXPECT_TRUE(grid.edgeB);
}
