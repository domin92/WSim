#pragma once

class AbstractSimulation {
public:
    struct ImageInfo {
        size_t width;
        size_t height;
        size_t totalSize;
    };

    virtual void stepSimulation(float dt) = 0;
    virtual ImageInfo getImageInfo2D() = 0;
    virtual void getImageData2D(void *data) = 0;
    virtual void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;
};
