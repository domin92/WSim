#pragma once

#include "Source/WSimMPI/master/MasterRendererInterface.h"

class Renderer;

class MasterRendererInterfaceWater2D : public MasterRendererInterface {
public:
    MasterRendererInterfaceWater2D(Master &master);
    constexpr static size_t mainBufferTexelSize = 4 * sizeof(float);

    // For MPI
    virtual void mainLoop() override;

    // For Renderer, called each frame
    virtual void stepSimulation(float dt) override;
    virtual size_t getSubImagesCount2D() override;
    virtual SubImageInfo getSubImageInfo2D(size_t subImageIndex) override;
    virtual void getSubImage2D(size_t subImageIndex, void *data) override;

private:
    std::unique_ptr<Renderer> createRenderer(Master &master);
    std::unique_ptr<Renderer> renderer;
};
