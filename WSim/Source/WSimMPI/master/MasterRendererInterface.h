#pragma once

#include "Source/WSimMPI/master/master.hpp"
#include "Source/WSimSimulation/Simulation/AbstractSimulation.h"

#include <memory>

// Glue between MPI and Renderer. It is meant to abstract the renderer
// interaction from the MPI part so renderers can be easily swapped.
// There should be one implementation of this interface for each new
// renderer added to the system.
class MasterRendererInterface : public AbstractSimulation {
public:
    MasterRendererInterface(Master &master) : master(master) {}

    // For MPI
    virtual void sendToNodesExtra(){};
    virtual void mainLoop() = 0;

    // For renderer
    virtual void stepSimulation(float dt) { master.receiveFromNodes(); }

protected:
    Master &master;
};
