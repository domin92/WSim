#include "LevelSetRenderer3D.hpp"

LevelSetRenderer::LevelSetRenderer(LevelSetRendererCallbacks &callbacks, int screenWidth, int screenHeight)
    : Renderer(GLFW_OPENGL_CORE_PROFILE, screenWidth, screenHeight),
      callbacks(callbacks) {
}

void LevelSetRenderer::processKeyboardInput(int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;
    }
}

void LevelSetRenderer::update(float deltaTimeSeconds) {
    callbacks.updateSimulation(deltaTimeSeconds);
}

void LevelSetRenderer::render() {
    float *levelSetData = callbacks.getData(); // render this

    glClearColor(0.1f, 0.5f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
