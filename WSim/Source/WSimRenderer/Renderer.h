#pragma once

#include "Source/WSimCommon/FpsCounter.h"
#include "Source/WSimRenderer/OpenGL.h"

#include <cassert>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>

class Renderer {
public:
    using FpsCallback = std::function<void(unsigned int)>;

    Renderer(int oglProfile, int width, int height);
    virtual ~Renderer();
    void setFpsCallback(FpsCallback fpsCallback);
    void mainLoop();

protected:
    static void callbackFrameBufferSize(GLFWwindow *window, int width, int height);
    static void callbackProcessInput(GLFWwindow *window, int button, int action, int mods);
    static void callbackMouseMove(GLFWwindow *window, double x, double y);

    virtual void processInput(int button, int action, int mods) = 0;
    virtual void processMouseMove(double x, double y) = 0;
    virtual void update(float deltaTimeSeconds) = 0;
    virtual void render() = 0;

    using Clock = std::chrono::steady_clock;
    GLFWwindow *window{};
    Clock::time_point lastFrameTime;

    // Fps measuring
    FpsCallback fpsCallback = {};
    std::unique_ptr<DefaultFpsCounter> fpsCounter = {};
};
