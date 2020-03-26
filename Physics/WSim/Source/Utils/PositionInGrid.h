#pragma once

#include "Utils/OpenCL.h"

struct PositionInGrid {
    struct PositionInGrid(OCL::Vec3 positionInGrid, OCL::Vec3 gridSize) {
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
    bool edgeL; // left
    bool edgeR; // right
    bool edgeU; // up
    bool edgeD; // down
    bool edgeF; // front
    bool edgeB; // back
};
