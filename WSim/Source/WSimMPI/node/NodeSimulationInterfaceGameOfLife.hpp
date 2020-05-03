#pragma once

#include "Source/WSimMPI/node/NodeSimulationInterface.hpp"

struct ShareBuffers;

class NodeSimulationInterfaceGameOfLife : public NodeSimulationInterface {
public:
    NodeSimulationInterfaceGameOfLife(Node &node);

    void preShareCopy() override;
    void iter() override;
    void postShareCopy() override;

private:
    void preShareCopyBuffer(char *output_buffer, int size_x, int size_y, int size_z, int out_x, int out_y, int out_z);
    void postShareCopyBuffer(char *input_buffer, int size_x, int size_y, int size_z, int out_x, int out_y, int out_z);
};
