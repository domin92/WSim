#pragma once

class Node;

class NodeSimulationInterface {
public:
    NodeSimulationInterface(Node& node);
    virtual ~NodeSimulationInterface();

    virtual void preShareCopy() = 0;
    virtual void iter() = 0;
    virtual void postShareCopy() = 0;

protected:
    Node& node;
};
