#pragma once

class Node;

class NodeSimulationInterface {
public:
    NodeSimulationInterface(Node &node);
    virtual ~NodeSimulationInterface();

    virtual void postReceiveFromMaster(const char *receivedArray) = 0;
    virtual void preSendToMaster(char *arrayToSend) = 0;
    virtual void preShareCopy() = 0;
    virtual void iter() = 0;
    virtual void postShareCopy() = 0;

protected:
    Node &node;
};
