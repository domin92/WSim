#pragma once

#include "Source/WSimMPI/Master/MasterRendererInterface.hpp"
#include "Source/WSimRenderer/ColorRenderer.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

class Renderer;

class MasterRendererInterface2D : public MasterRendererInterface {
public:
    MasterRendererInterface2D(Master &master);

    // For MPI
    virtual void mainLoop() override;

    // For Renderer, called each frame
    class ColorRendererCallbacksImpl : public ColorRendererCallbacks {
    public:
        ColorRendererCallbacksImpl(Master &master);
        size_t getSubImagesCount2D() override;
        SubImageInfo getSubImageInfo2D(size_t subImageIndex) override;
        void getSubImage2D(size_t subImageIndex, void *data) override;
        void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) override;
        void stop() override;
        void reset() override;
        void stepSimulation(float deltaTimeSeconds) override;

    private:
        Master &master;
    };

private:
    ColorRendererCallbacksImpl rendererCallbacks;
    std::unique_ptr<Renderer> renderer;
};
