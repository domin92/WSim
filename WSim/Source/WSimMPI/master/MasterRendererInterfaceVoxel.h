#pragma once

#include "Source/WSimMPI/master/MasterRendererInterface.h"

class MasterRendererInterfaceVoxel : public MasterRendererInterface {
public:
    MasterRendererInterfaceVoxel(Master &master);
    constexpr static size_t mainBufferTexelSize = 1;

private:
    std::unique_ptr<Renderer> createRenderer(Master &master);
};
