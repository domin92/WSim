#include "Source/WSimCommon/MainLooper.hpp"

#include <thread>

MainLooper::MainLooper(Clock::duration minimumFrameLength)
    : minimumFrameLength(minimumFrameLength),
      lastFrameTime(Clock::now()) {
}

void MainLooper::setFpsCallback(FpsCallback fpsCallback) {
    this->fpsCallback = fpsCallback;
    this->fpsCounter = std::make_unique<DefaultFpsCounter>();
}

void MainLooper::mainLoop() {
    while (!shouldClose()) {
        // Handle time
        const auto frameTime = Clock::now();
        const auto deltaTime = frameTime - lastFrameTime;
        if (minimumFrameLength > deltaTime) {
            const auto lackingDeltaTime = minimumFrameLength - deltaTime;
            std::this_thread::sleep_for(lackingDeltaTime);
        }
        lastFrameTime = frameTime;
        const auto deltaTimeFloat = std::chrono::duration_cast<std::chrono::duration<float>>(deltaTime).count();

        // Fps callback
        if (fpsCallback) {
            fpsCounter->push(deltaTime);
            fpsCallback(fpsCounter->getFps());
        }

        // Update
        preUpdate();
        update(deltaTimeFloat);
        postUpdate();
    }
}