#pragma once

#include "Source/WSimCommon/MainLooper.hpp"
#include "Source/WSimRenderer/OpenGL.hpp"

#include <iostream>

class Renderer : public MainLooper {
public:
    Renderer(int oglProfile, int width, int height);
    virtual ~Renderer();

protected:
    static void callbackFrameBufferSize(GLFWwindow *window, int width, int height);
    static void callbackKeyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void callbackProcessInput(GLFWwindow *window, int button, int action, int mods);
    static void callbackMouseMove(GLFWwindow *window, double x, double y);
    static void callbackScroll(GLFWwindow *window, double x, double y);

    virtual void processInput(int button, int action, int mods){};
    virtual void processKeyboardInput(int key, int scancode, int action, int mods){};
    virtual void processMouseMove(double x, double y){};
    virtual void processScroll(double xoffset, double yoffset){};

    void preUpdate() override;
    virtual void render() = 0;
    void postUpdate() override;
    bool shouldClose() override;

    GLFWwindow *window{};
};
