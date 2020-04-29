#include "ColorRenderer.h"

constexpr static int initialWidth = 600;
constexpr static int initialHeight = 600;

ColorRenderer::ColorRenderer(AbstractSimulation &simulation)
    : Renderer(GLFW_OPENGL_COMPAT_PROFILE, initialWidth, initialHeight),
      simulation(simulation),
      texture1Info(simulation.getImageInfo2D()),
      texture1Data(std::make_unique<char[]>(texture1Info.totalSize)) {
    glGenTextures(1, &texture1);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, initialWidth, 0, initialHeight, -1, 1);
    ASSERT_GL_NO_ERROR();
}

void ColorRenderer::processInput(int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        clicked = (action == GLFW_PRESS);
        if (clicked) {
            lastMouseX = transformCoordsFromAbsoluteSpaceToSimulationSpaceX(static_cast<float>(x));
            lastMouseY = transformCoordsFromAbsoluteSpaceToSimulationSpaceY(static_cast<float>(y));
        }
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        simulation.stop();
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        simulation.reset();
        break;
    }
}

void ColorRenderer::processMouseMove(double screenX, double screenY) {
    // Calculate position in simulation space and abort if it's out of bounds
    const float x = transformCoordsFromAbsoluteSpaceToSimulationSpaceX(static_cast<float>(screenX));
    const float y = transformCoordsFromAbsoluteSpaceToSimulationSpaceY(static_cast<float>(screenY));
    if (!clicked || x < 0 || y < 0 || x > simulation.getImageInfo2D().width || y > simulation.getImageInfo2D().height) {
        return;
    }

    // Calculate the mouse movement and abort if mouse did not move
    const float deltaX = x - lastMouseX;
    const float deltaY = y - lastMouseY;
    lastMouseX = x;
    lastMouseY = y;
    if (deltaX == 0 && deltaY == 0) {
        return;
    }

    // Apply force at given point
    const float radius = static_cast<float>(simulation.getImageInfo2D().width) / 10.f;
    simulation.applyForce(x, y, deltaX, deltaY, radius);
}

void ColorRenderer::update(float dt) {
    simulation.stepSimulation(dt);
    simulation.getImageData2D(texture1Data.get());

    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)texture1Info.width, (GLsizei)texture1Info.height, 0, GL_RGBA, GL_FLOAT, texture1Data.get());
    glBindTexture(GL_TEXTURE_2D, 0);
    ASSERT_GL_NO_ERROR();
}

void ColorRenderer::render() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2i(100, 100);
    glTexCoord2i(0, 1);
    glVertex2i(100, 500);
    glTexCoord2i(1, 1);
    glVertex2i(500, 500);
    glTexCoord2i(1, 0);
    glVertex2i(500, 100);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    ASSERT_GL_NO_ERROR();
}

float ColorRenderer::transformCoordsFromAbsoluteSpaceToSimulationSpaceX(double x) {
    // To texture space
    x -= 100;

    // To simulation space
    const auto simulationWidth = static_cast<float>(simulation.getImageInfo2D().width);
    const auto textureWidth = 400.f;
    return static_cast<float>(x) * (simulationWidth / textureWidth);
}

float ColorRenderer::transformCoordsFromAbsoluteSpaceToSimulationSpaceY(double y) {
    // To texture space
    y -= 100;
    y = 400 - y;

    // To simulation space
    const auto simulationHeight = static_cast<float>(simulation.getImageInfo2D().height);
    const auto textureHeight = 400.f;
    return static_cast<float>(y) * (simulationHeight / textureHeight);
}
