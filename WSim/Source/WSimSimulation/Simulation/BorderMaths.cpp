#include "BorderMaths.hpp"

#include "Source/WSimCommon/Error.hpp"

Vec3 increaseBorder(Vec3 simulationSize, PositionInGrid positionInGrid, int borderWidthChange) {
    const auto borderX = 2 - static_cast<int>(positionInGrid.edgeL) - static_cast<int>(positionInGrid.edgeR);
    const auto borderY = 2 - static_cast<int>(positionInGrid.edgeU) - static_cast<int>(positionInGrid.edgeD);
    const auto borderZ = 2 - static_cast<int>(positionInGrid.edgeF) - static_cast<int>(positionInGrid.edgeB);
    const Vec3 result = {
        simulationSize.x + borderX * borderWidthChange,
        simulationSize.y + borderY * borderWidthChange,
        simulationSize.z + borderZ * borderWidthChange};
    return result;
}

Vec3 decreaseBorder(Vec3 simulationSize, PositionInGrid positionInGrid, int borderWidthChange, Vec3 baseSimulationSize) {
    const Vec3 result = increaseBorder(simulationSize, positionInGrid, -borderWidthChange);
    wsimErrorUnless(baseSimulationSize.x <= result.x);
    wsimErrorUnless(baseSimulationSize.y <= result.y);
    wsimErrorUnless(baseSimulationSize.z <= result.z);
    return result;
}

Vec3 calculateBorderOffset(Vec3 totalSize, Vec3 usedSize, PositionInGrid positionInGrid) {
    const Vec3 borderSize = {
        totalSize.x - usedSize.x,
        totalSize.y - usedSize.y,
        totalSize.z - usedSize.z,
    };

    const auto calcualateOffsetComponent = [](bool edgeLower, bool edgeHigher, size_t borderSize) -> size_t {
        if (edgeLower) {
            return 0;
        } else {
            return borderSize / (2 - static_cast<size_t>(edgeHigher));
        }
    };

    const Vec3 result{
        calcualateOffsetComponent(positionInGrid.edgeL, positionInGrid.edgeR, borderSize.x),
        calcualateOffsetComponent(positionInGrid.edgeU, positionInGrid.edgeD, borderSize.y),
        calcualateOffsetComponent(positionInGrid.edgeF, positionInGrid.edgeB, borderSize.z),
    };
    return result;
}
