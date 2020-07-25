#pragma once

#include <memory>

class MasterRendererInterface;

class Master {
    // Sizes
    const int gridSize;
    const int nodeSize;
    const int fullSize;
    const int procCount;
    const int nodeVolume;

    // Allocations
    char **mappedBuffer;
    char *mainBuffer;

    // Renderer
    std::unique_ptr<MasterRendererInterface> rendererInterface;

public:
    // General
    Master(int proc_count, int grid_size, int nodeSize);
    ~Master();
    void main();

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
