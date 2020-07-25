#include "MasterRendererInterface2D.hpp"

#include "Source/WSimMPI/Utils.hpp"

#include <cstring>

MasterRendererInterface2D::MasterRendererInterface2D(Master &master)
    : MasterRendererInterface(master),
      rendererCallbacks(master),
      renderer(std::make_unique<ColorRenderer>(rendererCallbacks, sizeof(float) * 4)) {
}

void MasterRendererInterface2D::mainLoop() {
    renderer->mainLoop();
}

MasterRendererInterface2D::ColorRendererCallbacksImpl::ColorRendererCallbacksImpl(Master &master) : master(master) {}

size_t MasterRendererInterface2D::ColorRendererCallbacksImpl::getSubImagesCount2D() {
    return static_cast<size_t>(master.getProcCount()) - 1u;
}

ColorRendererCallbacks::SubImageInfo MasterRendererInterface2D::ColorRendererCallbacksImpl::getSubImageInfo2D(size_t subImageIndex) {
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

void MasterRendererInterface2D::ColorRendererCallbacksImpl::getSubImage2D(size_t subImageIndex, void *data) {
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

void MasterRendererInterface2D::ColorRendererCallbacksImpl::applyForce(float positionX, float positionY, float changeX, float changeY, float radius) {
    wsimError();
}

void MasterRendererInterface2D::ColorRendererCallbacksImpl::stop() {
    wsimError();
}

void MasterRendererInterface2D::ColorRendererCallbacksImpl::reset() {
    wsimError();
}

void MasterRendererInterface2D::ColorRendererCallbacksImpl::stepSimulation(float deltaTimeSeconds) {
    master.receiveFromNodes();
}
