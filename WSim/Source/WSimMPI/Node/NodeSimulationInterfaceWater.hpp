#pragma once

#include "Source/WSimCommon/SimulationMode.h"
#include "Source/WSimMPI/Node/NodeSimulationInterface.hpp"
#include "Source/WSimMPI/Node/OclCopyHelper.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

struct ShareBuffers;

class NodeSimulationInterfaceWater : public NodeSimulationInterface {
public:
    NodeSimulationInterfaceWater(Node &node, SimulationMode simulationMode);

    void postReceiveFromMaster(const uint8_t *receivedArray) override;
    void preSendToMaster(uint8_t *arrayToSend) override;
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
