#pragma once

#include "Source/WSimCommon/Vec3.h"

struct PositionInGrid {
    PositionInGrid() {}
    PositionInGrid(Vec3 positionInGrid, Vec3 gridSize) {
        isBorder(positionInGrid.x, gridSize.x, edgeL, edgeR);
        isBorder(positionInGrid.y, gridSize.y, edgeU, edgeD);
        isBorder(positionInGrid.z, gridSize.z, edgeF, edgeB);
    }

    static void isBorder(size_t position, size_t size, bool &lower, bool &upper) {
        lower = (position == 0);
        upper = (position == size - 1);
    }

    // If for example edgeL is set, then it means the simulation is being performed on the leftmost
    // (logically) node, meaning there's no neighbouring node to the left. Extended allocations do
    // not have border space to the left.
    bool edgeL = false; // left
    bool edgeR = false; // right
    bool edgeU = false; // up
    bool edgeD = false; // down
    bool edgeF = false; // front
    bool edgeB = false; // back
};

Vec3 increaseBorder(Vec3 simulationSize, PositionInGrid positionInGrid, int borderWidthChange);
Vec3 decreaseBorder(Vec3 simulationSize, PositionInGrid positionInGrid, int borderWidthChange, Vec3 baseSimulationSize);
Vec3 calculateBorderOffset(Vec3 totalSize, Vec3 usedSize, PositionInGrid positionInGrid);
