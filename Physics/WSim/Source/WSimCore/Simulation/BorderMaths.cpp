#include "BorderMaths.h"

#include <assert.h>

OCL::Vec3 increaseBorder(OCL::Vec3 simulationSize, PositionInGrid positionInGrid, int borderWidthChange) {
    const auto borderX = 2 - static_cast<int>(positionInGrid.edgeL) - static_cast<int>(positionInGrid.edgeR);
    const auto borderY = 2 - static_cast<int>(positionInGrid.edgeD) - static_cast<int>(positionInGrid.edgeU);
    const auto borderZ = 2 - static_cast<int>(positionInGrid.edgeF) - static_cast<int>(positionInGrid.edgeB);
    const OCL::Vec3 result = {
        simulationSize.x + borderX * borderWidthChange,
        simulationSize.y + borderY * borderWidthChange,
        simulationSize.z + borderZ * borderWidthChange};
    return result;
}

OCL::Vec3 decreaseBorder(OCL::Vec3 simulationSize, PositionInGrid positionInGrid, int borderWidthChange, OCL::Vec3 baseSimulationSize) {
    const OCL::Vec3 result = increaseBorder(simulationSize, positionInGrid, -borderWidthChange);
    assert(baseSimulationSize.x <= result.x);
    assert(baseSimulationSize.y <= result.y);
    assert(baseSimulationSize.z <= result.z);
    return result;
}

OCL::Vec3 calculateBorderOffset(OCL::Vec3 totalSize, OCL::Vec3 usedSize, PositionInGrid positionInGrid) {
    const OCL::Vec3 borderSize = {
        totalSize.x - usedSize.x,
        totalSize.y - usedSize.y,
        totalSize.z - usedSize.z,
    };

    const auto calcualateOffsetComponent = [](bool edgeLower, bool edgeHigher, size_t borderSize) -> size_t {
        if (edgeLower) {
            return 0;
        }
        else {
            return borderSize / (2 - static_cast<size_t>(edgeHigher));
        }
    };

    const OCL::Vec3 result{
        calcualateOffsetComponent(positionInGrid.edgeL, positionInGrid.edgeR, borderSize.x),
        calcualateOffsetComponent(positionInGrid.edgeD, positionInGrid.edgeU, borderSize.y),
        calcualateOffsetComponent(positionInGrid.edgeF, positionInGrid.edgeB, borderSize.z),
    };
    return result;
}
