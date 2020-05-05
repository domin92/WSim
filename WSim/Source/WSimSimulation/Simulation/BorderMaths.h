#pragma once

#include "Source/WSimSimulation/Utils/OpenCL.h"

struct PositionInGrid {
    PositionInGrid() {}
    PositionInGrid(OCL::Vec3 positionInGrid, OCL::Vec3 gridSize) {
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

OCL::Vec3 increaseBorder(OCL::Vec3 simulationSize, PositionInGrid positionInGrid, int borderWidthChange);
OCL::Vec3 decreaseBorder(OCL::Vec3 simulationSize, PositionInGrid positionInGrid, int borderWidthChange, OCL::Vec3 baseSimulationSize);
OCL::Vec3 calculateBorderOffset(OCL::Vec3 totalSize, OCL::Vec3 usedSize, PositionInGrid positionInGrid);
