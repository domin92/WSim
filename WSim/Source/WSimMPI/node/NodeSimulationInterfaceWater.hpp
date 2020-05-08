#pragma once

#include "Source/WSimMPI/node/NodeSimulationInterface.hpp"
#include "Source/WSimMPI/node/OclCopyHelper.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"

struct ShareBuffers;

class NodeSimulationInterfaceWater : public NodeSimulationInterface {
public:
    NodeSimulationInterfaceWater(Node &node);
    constexpr static size_t texelSize = 4 * sizeof(float);
    constexpr static size_t mainArraysCount = 2;

    void postReceiveFromMaster(const char *receivedArray) override;
    void preSendToMaster(char *arrayToSend) override;
    void preShareCopy() override;
    void iter() override;
    void postShareCopy() override;

private:
    static PositionInGrid createPositionInGrid(Node &node);
    static OCL::Vec3 getNodeSize(Node &node);

    PositionInGrid positionInGrid;
    Simulation simulation;
    OclCopyHelper copier;
};
