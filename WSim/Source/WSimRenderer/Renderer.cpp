#include "Renderer.hpp"

#include <iostream>
#include <thread>

Renderer::Renderer(int oglProfile, int width, int height)
    : MainLooper(std::chrono::milliseconds(16)) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    if (oglProfile == GLFW_OPENGL_COMPAT_PROFILE) {
        // We can use OpenGL 3.1 for compatibility profile.
        // This version does not support profile selection
        // though, hence the lack of GLFW_OPENGL_PROFILE hint.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    } else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, oglProfile);
    }

    window = glfwCreateWindow(width, height, "WSim", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        const char *log = nullptr;
        glfwGetError(&log);
        std::cout << log << std::endl;
        std::abort();
    }
    glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, callbackFrameBufferSize);

    glfwSetKeyCallback(window, callbackKeyboardInput);
    glfwSetMouseButtonCallback(window, callbackProcessInput);
    glfwSetCursorPosCallback(window, callbackMouseMove);
    glfwSetScrollCallback(window, callbackScroll);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        std::abort();
    }
}

Renderer::~Renderer() {
    glfwTerminate();
}

void Renderer::preUpdate() {
    // Clear window
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
};

void Renderer::postUpdate() {
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
}
bool Renderer::shouldClose() {
    return glfwWindowShouldClose(window);
};

void Renderer::callbackFrameBufferSize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::callbackKeyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->processKeyboardInput(key, scancode, action, mods);
}

void Renderer::callbackProcessInput(GLFWwindow *window, int button, int action, int mods) {
    auto renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->processInput(button, action, mods);
}

void Renderer::callbackMouseMove(GLFWwindow *window, double x, double y) {
    auto renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->processMouseMove(x, y);
}

void Renderer::callbackScroll(GLFWwindow *window, double x, double y) {
    auto renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->processScroll(x, y);
}
