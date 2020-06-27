#pragma once

#include "Source/WSimMPI/master/master.hpp"

#include <memory>
#include <chrono>
#include <iostream>
#include "Source/WSimCommon/Error.h"

#include "Source/WSimRenderer/FpsCounter.h"

struct ColorRendererCallbacks {
    struct SubImageInfo {
        size_t xOffset;
        size_t yOffset;
        size_t width;
        size_t height;
        bool valid;
    };
    virtual size_t getSubImagesCount2D() = 0;
    virtual SubImageInfo getSubImageInfo2D(size_t subImageIndex) = 0;
    virtual void getSubImage2D(size_t subImageIndex, void *data) = 0;
    virtual void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;
    virtual void stepSimulation(float deltaTimeSeconds) = 0;
};

struct VolumeRendererCallbacks {
    virtual void stepSimulation(float deltaTimeSeconds) = 0;
    virtual char *getVolumeBuffers() = 0;
};

// Glue between MPI and Renderer. It is meant to abstract the renderer
// interaction from the MPI part so renderers can be easily swapped.
// There should be one implementation of this interface for each new
// renderer added to the system.
class Renderer{};
class MasterRendererInterface {
public:
    MasterRendererInterface(Master &master) : master(master) {}

    // For MPI
    virtual void sendToNodesExtra(){};
    virtual void mainLoop() = 0;

protected:
    Master &master;
};
