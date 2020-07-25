#pragma once

#include "Source/WSimCommon/Logger.hpp"

#include <chrono>

class DefaultFpsCallback {
public:
    using Clock = std::chrono::steady_clock;
    DefaultFpsCallback() : DefaultFpsCallback(std::chrono::milliseconds(700)) {}
    DefaultFpsCallback(Clock::duration printFrequency) : printFrequency(printFrequency) {}

    void operator()(unsigned int fps) {
        const Clock::time_point now = Clock::now();
        if (now - lastPrintTime > printFrequency) {
            Logger::get() << "FPS: " << fps << '\n';
            lastPrintTime = now;
        }
    }

private:
    const Clock::duration printFrequency;
    Clock::time_point lastPrintTime = {};
};