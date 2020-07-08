#pragma once

#include "Source/WSimRenderer/FpsCounter.h"

#include <chrono>
#include <functional>
#include <memory>

class MainLooper {
public:
    using Clock = std::chrono::steady_clock;
    using FpsCallback = std::function<void(unsigned int)>;

    MainLooper(Clock::duration minimumFrameLength);

    void setFpsCallback(FpsCallback fpsCallback);
    void mainLoop();

protected:
    virtual void preUpdate() {}
    virtual void update(float deltaTime) = 0;
    virtual void postUpdate() {}
    virtual bool shouldClose() { return false; }

private:
    const Clock::duration minimumFrameLength;
    Clock::time_point lastFrameTime;

    // Fps measuring
    FpsCallback fpsCallback = {};
    std::unique_ptr<DefaultFpsCounter> fpsCounter = {};
};
