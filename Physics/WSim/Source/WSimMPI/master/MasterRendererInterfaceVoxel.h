#pragma once

#include "Source/WSimMPI/master/MasterRendererInterface.h"

class MasterRendererInterfaceVoxel : public MasterRendererInterface {
public:
    MasterRendererInterfaceVoxel(Master &master);

    // For MPI, called to initialize buffers on nodes
    virtual void sendToNodes() override;

    // For Renderer, called each frame
    virtual void stepSimulation(float dt) override;

private:
    std::unique_ptr<Renderer> createRenderer(Master &master);
};
