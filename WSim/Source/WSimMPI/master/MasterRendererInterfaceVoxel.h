#pragma once

#include "Source/WSimMPI/master/MasterRendererInterface.h"

class Renderer;

class MasterRendererInterfaceVoxel : public MasterRendererInterface {
public:
    MasterRendererInterfaceVoxel(Master &master);
    constexpr static size_t mainBufferTexelSize = 1;

    virtual void mainLoop() override;

private:
    std::unique_ptr<Renderer> createRenderer(Master &master);

    std::unique_ptr<Renderer> renderer;
};
