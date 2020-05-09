#pragma once

#include "Source/WSimCommon/Error.h"

class AbstractSimulation {
public:
    // General calls
    virtual void stepSimulation(float dt) = 0;
    virtual void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) { wsimError(); }
    virtual void stop() { wsimError(); }
    virtual void reset() { wsimError(); }
};
