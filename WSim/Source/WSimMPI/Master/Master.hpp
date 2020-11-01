#pragma once

#include "Source/WSimCommon/SimulationMode.h"

#include <cstdint>
#include <memory>

class MasterRendererInterface;
struct SimulationMode;

class Master {
    // Sizes
    const int gridSize;
    const int nodeSize;
    const int fullSize;
    const int procCount;
    const int nodeVolume;

    // Allocations
    uint8_t **mappedBuffer;
    uint8_t *mainBuffer;

    // Renderer
    SimulationMode simulationMode;
    std::unique_ptr<MasterRendererInterface> rendererInterface;

public:
    // General
    Master(int procCount, int gridSizeInNodes, int nodeSize, SimulationMode simulationMode);
    ~Master();
    void main();

    // Creation helpers
    std::unique_ptr<MasterRendererInterface> createRendererInterface(SimulationMode simulationMode);

    // Nodes communication
    void sendToNodes();
    void receiveFromNodes();

    // Getters
    auto getGridSize() const { return gridSize; }
    auto getNodeSize() const { return nodeSize; }
    auto getFullSize() const { return fullSize; }
    auto getProcCount() const { return procCount; }
    auto getNodeVolume() const { return nodeVolume; }
    auto getMainBuffer() const { return mainBuffer; }
    auto getMappedBuffer() const { return mappedBuffer; }
};
