#pragma once

#include "Source/WSimRenderer/ColorRenderer.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"

class ColorRendererCallbacksImpl : public ColorRendererCallbacks {
public:
    ColorRendererCallbacksImpl(Simulation &simulation) : simulation(simulation) {}

    size_t getSubImagesCount2D() override {
        return 1u;
    }

    SubImageInfo getSubImageInfo2D(size_t subImageIndex) override {
        SubImageInfo info;
        info.xOffset = 0;
        info.yOffset = 0;
        info.width = simulation.getSimulationSize().x;
        info.height = simulation.getSimulationSize().y;
        info.valid = true;
        return info;
    }

    void getSubImage2D(size_t subImageIndex, void *data) override {
        wsimErrorIf(subImageIndex != 0);
        Vec3 offset = simulation.getBorderOffset();
        offset.z = 0;
        Vec3 size = simulation.getSimulationSize();
        size.z = 1;
        OCL::enqueueReadImage3D(simulation.getCommandQueue(), simulation.getColor().getSource(), CL_BLOCKING, offset, size, data);
    }

    void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) override {
        simulation.applyForce(positionX, positionY, changeX, changeY, radius);
    }

    void stop() override {
        simulation.stop();
    }

    void reset() override {
        simulation.reset();
    }

    void stepSimulation(float deltaTimeSeconds) override {
        simulation.stepSimulation(deltaTimeSeconds);
    }

private:
    Simulation &simulation;
};
