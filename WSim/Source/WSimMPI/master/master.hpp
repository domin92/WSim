#ifndef MASTER_HPP
#define MASTER_HPP

#include <memory>

class MasterRendererInterface;

class Master {
    // Sizes
    const int grid_size;
    const int node_size;
    const int full_size;
    const int proc_count;
    const int node_volume;

    // Allocations
    char **mapped_buffer;
    char *main_buffer;

    // Renderer
    std::unique_ptr<MasterRendererInterface> rendererInterface;

public:
    // General
    Master(int proc_count, int grid_size, int node_size);
    ~Master();
    void main();

    // Nodes communication
    void sendToNodes();
    void receiveFromNodes();

    // Getters
    auto getGridSize() const { return grid_size; }
    auto getNodeSize() const { return node_size; }
    auto getFullSize() const { return full_size; }
    auto getProcCount() const { return proc_count; }
    auto getNodeVolume() const { return node_volume; }
    auto getMainBuffer() const { return main_buffer; }
    auto getMappedBuffer() const { return mapped_buffer; }
};

#endif
