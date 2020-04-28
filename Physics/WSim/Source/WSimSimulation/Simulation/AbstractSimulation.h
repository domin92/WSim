#pragma once

#include "Source/WSimSimulation/Utils/Error.h"

class AbstractSimulation {
public:
    struct ImageInfo {
        size_t width;
        size_t height;
        size_t totalSize;
    };

    virtual void stepSimulation(float dt) = 0;
    virtual ImageInfo getImageInfo2D() { wsimError(); }
    virtual void getImageData2D(void *data) { wsimError(); }
    virtual void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) { wsimError(); }
    virtual void stop() { wsimError(); }
    virtual void reset() { wsimError(); }
};
