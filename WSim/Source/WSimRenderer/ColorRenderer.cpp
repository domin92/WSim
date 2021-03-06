#include "ColorRenderer.hpp"

#include <algorithm>

constexpr static int initialWidth = 600;
constexpr static int initialHeight = 600;

ColorRenderer::ColorRenderer(ColorRendererCallbacks &callbacks, size_t voxelSize)
    : Renderer(GLFW_OPENGL_COMPAT_PROFILE, initialWidth, initialHeight),
      callbacks(callbacks) {
    // Prepare subImages info and data
    const auto subImagesCount = callbacks.getSubImagesCount2D();
    this->subImagesInfo.reserve(subImagesCount);
    this->subImagesData.reserve(subImagesCount);
    for (auto i = 0u; i < subImagesCount; i++) {
        const auto info = callbacks.getSubImageInfo2D(i);
        const auto size = info.width * info.height * voxelSize;
        this->subImagesInfo.push_back(info);
        this->subImagesData.push_back(std::make_unique<char[]>(size));

        this->imageWidth = std::max(this->imageWidth, info.xOffset + info.width);
        this->imageHeight = std::max(this->imageHeight, info.yOffset + info.height);
    }

    // Setup OpenGL stuff
    CHECK_GL_ERROR(glGenTextures(1, &texture1))
    CHECK_GL_ERROR(glMatrixMode(GL_PROJECTION))
    CHECK_GL_ERROR(glOrtho(0, initialWidth, 0, initialHeight, -1, 1))

    // Initialize full image (because glTexSubImage2D is used later)
    auto a = std::make_unique<uint8_t[]>(imageWidth * imageHeight * voxelSize * 4);
    CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, texture1))
    CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(imageWidth), static_cast<GLsizei>(imageHeight), 0, GL_RGBA, GL_FLOAT, a.get()))
    CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0))
}

void ColorRenderer::processInput(int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    switch (button) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_MOUSE_BUTTON_LEFT:
        clicked = (action == GLFW_PRESS);
        if (clicked) {
            lastMouseX = transformCoordsFromAbsoluteSpaceToSimulationSpaceX(static_cast<float>(x));
            lastMouseY = transformCoordsFromAbsoluteSpaceToSimulationSpaceY(static_cast<float>(y));
        }
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        callbacks.stop();
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        callbacks.reset();
        break;
    }
}

void ColorRenderer::processMouseMove(double screenX, double screenY) {
    // Calculate position in simulation space and abort if it's out of bounds
    const float x = transformCoordsFromAbsoluteSpaceToSimulationSpaceX(static_cast<float>(screenX));
    const float y = transformCoordsFromAbsoluteSpaceToSimulationSpaceY(static_cast<float>(screenY));
    if (!clicked || x < 0 || y < 0 || x > imageWidth || y > imageHeight) {
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
    const float radius = static_cast<float>(imageWidth) / 10.f;
    callbacks.applyForce(x, y, deltaX, deltaY, radius);
}

void ColorRenderer::processKeyboardInput(int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;
    }
}

void ColorRenderer::update(float deltaTimeSeconds) {
    callbacks.stepSimulation(deltaTimeSeconds);
    CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, texture1))

    CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST))
    CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST))
    for (auto i = 0u; i < subImagesData.size(); i++) {
        const auto &info = subImagesInfo[i];
        if (!info.valid) {
            continue;
        }
        const auto &data = subImagesData[i];

        callbacks.getSubImage2D(i, data.get());
        ASSERT_GL_NO_ERROR();

        const auto xOffset = static_cast<GLint>(info.xOffset);
        const auto yOffset = static_cast<GLint>(info.yOffset);
        const auto width = static_cast<GLsizei>(info.width);
        const auto height = static_cast<GLsizei>(info.height);
        CHECK_GL_ERROR(glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, width, height, GL_BLUE, GL_FLOAT, data.get()))
    }

    CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0))
}

void ColorRenderer::render() {
    CHECK_GL_ERROR(glEnable(GL_TEXTURE_2D));
    CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, texture1));

    glBegin(GL_QUADS);
    glTexCoord2i(0, 1);
    glVertex2i(100, 100);
    glTexCoord2i(0, 0);
    glVertex2i(100, 500);
    glTexCoord2i(1, 0);
    glVertex2i(500, 500);
    glTexCoord2i(1, 1);
    glVertex2i(500, 100);
    CHECK_GL_ERROR(glEnd())

    CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
    CHECK_GL_ERROR(glDisable(GL_TEXTURE_2D))
}

float ColorRenderer::transformCoordsFromAbsoluteSpaceToSimulationSpaceX(double x) {
    // To texture space
    x -= 100;

    // To simulation space
    const auto simulationWidth = static_cast<float>(imageWidth);
    const auto textureWidth = 400.f;
    return static_cast<float>(x) * (simulationWidth / textureWidth);
}

float ColorRenderer::transformCoordsFromAbsoluteSpaceToSimulationSpaceY(double y) {
    // To texture space
    y -= 100;

    // To simulation space
    const auto simulationHeight = static_cast<float>(imageHeight);
    const auto textureHeight = 400.f;
    return static_cast<float>(y) * (simulationHeight / textureHeight);
}
