#pragma once
#include <cstdint>

class Node;

class NodeSimulationInterface {
public:
    NodeSimulationInterface(Node &node);
    virtual ~NodeSimulationInterface();

    virtual void postReceiveFromMaster(const uint8_t *receivedArray) = 0;
    virtual void preSendToMaster(uint8_t *arrayToSend) = 0;
    virtual void preShareCopy() = 0;
    virtual void iter() = 0;
    virtual void postShareCopy() = 0;

protected:
    Node &node;
};
