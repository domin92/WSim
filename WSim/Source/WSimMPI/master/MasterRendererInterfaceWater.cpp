#include "MasterRendererInterfaceWater.hpp"

#include "Source/WSimMPI/Utils.h"
#include "Source/WSimRenderer/ColorRenderer.h"

MasterRendererInterfaceWater2D::MasterRendererInterfaceWater2D(Master &master)
    : MasterRendererInterface(master),
      renderer(createRenderer(master)) {
}

void MasterRendererInterfaceWater2D::mainLoop() {
    renderer->mainLoop();
}

void MasterRendererInterfaceWater2D::stepSimulation(float dt) {
    MasterRendererInterface::stepSimulation(dt);
}

size_t MasterRendererInterfaceWater2D::getSubImagesCount2D() {
    return static_cast<size_t>(master.getProcCount()) - 1u;
}

MasterRendererInterfaceWater2D::SubImageInfo MasterRendererInterfaceWater2D::getSubImageInfo2D(size_t subImageIndex) {
    SubImageInfo info{};
    const auto nodeRank = subImageIndex + 1; // convert methods expect process ranks
    if (convertTo3DRankZ(nodeRank, master.getGridSize()) != 0) {
        // ColorRenderer is 2D renderer, we ignore other planes
        info.valid = false;
    } else {
        info.valid = true;
        info.xOffset = convertTo3DRankX(nodeRank, master.getGridSize()) * master.getNodeSize();
        info.yOffset = convertTo3DRankY(nodeRank, master.getGridSize()) * master.getNodeSize();
        info.width = master.getNodeSize();
        info.height = master.getNodeSize();
    }
    return info;
}

void MasterRendererInterfaceWater2D::getSubImage2D(size_t subImageIndex, void *data) {
    const auto gridSize = master.getGridSize();
    const auto nodeRank = subImageIndex + 1; // convert methods expect process ranks
    const auto xInGrid = convertTo3DRankX(nodeRank, master.getGridSize());
    const auto yInGrid = convertTo3DRankY(nodeRank, master.getGridSize());
    const auto zInGrid = convertTo3DRankZ(nodeRank, master.getGridSize());
    wsimErrorUnless(zInGrid == 0); // Renderer shouldn't call this method for back planes

    const char *buffer = master.getMappedBuffer()[subImageIndex];
    const auto nodeArea = master.getNodeVolume() / master.getNodeSize(); // We only copy first plane (z=0)
    memcpy(data, buffer, nodeArea);
}

std::unique_ptr<Renderer> MasterRendererInterfaceWater2D::createRenderer(Master &master) {
    return std::make_unique<ColorRenderer>(*this);
}
