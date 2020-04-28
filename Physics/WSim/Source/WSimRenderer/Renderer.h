#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format off

#include <cassert>
#include <iostream>
#include <iomanip>
#include <chrono>

#define ASSERT_GL_NO_ERROR()                                                     \
    {                                                                            \
        auto error = glGetError();                                               \
        if (error != GL_NO_ERROR) {                                              \
        std::cerr << "glGetError = 0x" << std::hex << error << std::dec << '\n'; \
            assert(false);                                                       \
        }                                                                        \
    }

class Renderer {
public:
    Renderer(int oglProfile, int width, int height);
    virtual ~Renderer();
    void mainLoop();

protected:
    static void callbackFrameBufferSize(GLFWwindow* window, int width, int height);
    static void callbackProcessInput(GLFWwindow* window, int button, int action, int mods);
    static void callbackMouseMove(GLFWwindow* window, double x, double y);

    virtual void processInput(int button, int action, int mods) = 0;
    virtual void processMouseMove(double x, double y) = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;

    GLFWwindow* window{};
    using Clock = std::chrono::steady_clock;
    Clock::time_point lastFrameTime;
};
