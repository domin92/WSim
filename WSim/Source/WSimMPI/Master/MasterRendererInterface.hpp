#pragma once

#include "Source/WSimMPI/Master/Master.hpp"

#include <memory>

// Glue between MPI and Renderer. It is meant to abstract the renderer
// interaction from the MPI part so renderers can be easily swapped.
// There should be one implementation of this interface for each new
// renderer added to the system.
class MasterRendererInterface {
public:
    MasterRendererInterface(Master &master) : master(master) {}

    // For MPI
    virtual void sendToNodesExtra(){};
    virtual void mainLoop() = 0;

protected:
    Master &master;
};
