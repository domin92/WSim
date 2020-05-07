#pragma once

#include "Source/WSimCommon/Error.h"

class AbstractSimulation {
public:
    // General calls
    virtual void stepSimulation(float dt) = 0;
    virtual void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) { wsimError(); }
    virtual void stop() { wsimError(); }
    virtual void reset() { wsimError(); }

    // Info for ColorRenderer
    struct SubImageInfo {
        size_t xOffset;
        size_t yOffset;
        size_t width;
        size_t height;
        bool valid;
    };
    virtual size_t getSubImagesCount2D() { wsimError(); }
    virtual SubImageInfo getSubImageInfo2D(size_t subImageIndex) { wsimError(); }
    virtual void getSubImage2D(size_t subImageIndex, void *data) { wsimError(); }
};
