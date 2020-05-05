#pragma once

#include "Source/WSimMPI/master/master.hpp"
#include "Source/WSimRenderer/Renderer.h"
#include "Source/WSimSimulation/Simulation/AbstractSimulation.h"

#include <memory>

// Glue between MPI and Renderer. It is meant to abstract the renderer
// interaction from the MPI part so renderers can be easily swapped.
// There should be one implementation of this interface for each new
// renderer added to the system.
class MasterRendererInterface : public AbstractSimulation {
public:
    MasterRendererInterface(Master &master, std::unique_ptr<Renderer> &&renderer)
        : master(master),
          renderer(std::move(renderer)) {}

    // For MPI
    virtual void sendToNodesExtra(){};
    void mainLoop() { renderer->mainLoop(); }

    // For renderer
    virtual void stepSimulation(float dt) { master.receiveFromNodes(); }

protected:
    Master &master;
    std::unique_ptr<Renderer> renderer;
};
