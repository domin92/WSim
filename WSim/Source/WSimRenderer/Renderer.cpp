#include "Renderer.h"

#include <iostream>
#include <thread>

Renderer::Renderer(int oglProfile, int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, oglProfile);

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
    glfwSetMouseButtonCallback(window, callbackProcessInput);
    glfwSetCursorPosCallback(window, callbackMouseMove);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        std::abort();
    }

    lastFrameTime = Clock::now();
}

Renderer::~Renderer() {
    glfwTerminate();
}

void Renderer::setFpsCallback(FpsCallback fpsCallback) {
    this->fpsCallback = fpsCallback;
    this->fpsCounter = std::make_unique<DefaultFpsCounter>();
}

void Renderer::mainLoop() {

    while (!glfwWindowShouldClose(window)) {
        // Handle time
        const auto frameTime = Clock::now();
        const auto deltaTime = frameTime - lastFrameTime;
        constexpr static auto minimumFrameLength = std::chrono::milliseconds(16);
        if (minimumFrameLength > deltaTime) {
            auto lackingDeltaTime = minimumFrameLength - deltaTime;
            std::this_thread::sleep_for(lackingDeltaTime);
        }
        lastFrameTime = frameTime;
        const auto deltaTimeFloat = std::chrono::duration_cast<std::chrono::duration<float>>(deltaTime).count();

        // Fps callback
        if (fpsCallback) {
            fpsCounter->push(deltaTime);
            fpsCallback(fpsCounter->getFps());
        }

        // Clear window
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update
        update(deltaTimeFloat);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Renderer::callbackFrameBufferSize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::callbackProcessInput(GLFWwindow *window, int button, int action, int mods) {
    auto renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->processInput(button, action, mods);
}

void Renderer::callbackMouseMove(GLFWwindow *window, double x, double y) {
    auto renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->processMouseMove(x, y);
}
