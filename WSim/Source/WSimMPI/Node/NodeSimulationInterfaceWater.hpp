#pragma once

#include "Source/WSimMPI/Node/NodeSimulationInterface.hpp"
#include "Source/WSimMPI/Node/OclCopyHelper.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

struct ShareBuffers;

class NodeSimulationInterfaceWater : public NodeSimulationInterface {
public:
    NodeSimulationInterfaceWater(Node &node);
    constexpr static size_t shareThickness = 15u;
    constexpr static size_t texelSize = Simulation::colorVoxelSize;
    constexpr static size_t mainArraysCount = 2;

    void postReceiveFromMaster(const char *receivedArray) override;
    void preSendToMaster(char *arrayToSend) override;
    void preShareCopy() override;
    void iter() override;
    void postShareCopy() override;

private:
    static PositionInGrid createPositionInGrid(Node &node);
    static Vec3 getNodeSize(Node &node);

    PositionInGrid positionInGrid;
    Simulation simulation;
    OclCopyHelper copier;
};
